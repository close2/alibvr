import 'dart:io';

import 'package:logging/logging.dart';

import 'package:args/args.dart';
import 'package:doc_code_collector/snippets/snippets.dart';

/**
 * This script extracts code snippets from code{Files,Dirs}.
 * The snippets must start with a name enclosed in « and » optionally followed
 * by [x,y] → x being a regular expression and y it's replacement optionally
 * followed by by *\/
 * Everything after the » (or *\/) is the considered snippet code up to the
 * next ¤ optionally enclosed by /\* and *\/.
 * The regular expression -- x to y -- replacement is then applied to the
 * extracted text.
 *
 * All files from docSources are copied to dst and all
 * +++snippetNames+++ are replaced by the snipped codes.
 * Optionally a regular expression may be added before the 2nd +++
 * +++snippetName[regFrom,regTo]+++
 *
 * The regular expression replacement is then applied to the snipped before
 * inserting it.
 */

ArgParser buildArgParser() {
  return new ArgParser()
    ..addOption('code',
        abbr: 'c',
        help: 'File or directory where snippets should be extracted from',
        allowMultiple: true)
    ..addOption('dst',
        abbr: 'd',
        help: 'Destination directory where src-documentation is copied to.  '
            'Copied files will have the snippets from code injected.')
    ..addFlag('deleteDst',
        abbr: 'D',
        help: 'If dst exists delete and recreate it.',
        defaultsTo: false)
    ..addOption('src',
        abbr: 's',
        help: 'File or directory of the documentation which will then copied '
            'to dst.  The copied files will have the snippets injected.',
        allowMultiple: true)
    ..addFlag('reduceSrcPathForCopy',
        abbr: 'r',
        help: 'When copying documentation source files, reduce the path of src'
            'to the last file / directory element.',
        defaultsTo: true)
    ..addFlag('help',
        abbr: 'h',
        help: 'Display this help.',
        negatable: false,
        defaultsTo: false)
    ..addOption('textFileExt',
        abbr: 't',
        help: 'Only selected file endings are considered as text files. '
            'All other files are ignored while looking for or inject '
            'snippets.  You may add other endings here.  Example: «.doc»',
        allowMultiple: true)
    ..addFlag('cleanSrcFromAnnotations',
        help: 'When copying documentation source files, remove annotations of '
            'code snippets.',
        defaultsTo: true)
    ..addFlag('verbose', abbr: 'v', help: 'Display debug output.');
}

void printUsage(ArgParser parser) {
  print('Extracts snippets from code files / directories');
  print('  and injects them into documentation files, which are');
  print('  copied to a destination directory.');
  print('');
  print(parser.usage);
}

main(List<String> args) {
  Logger.root.level = Level.SEVERE;
  Logger.root.onRecord.listen((LogRecord rec) {
    print('${rec.level.name}: ${rec.time}: ${rec.message}');
  });

  var parser = buildArgParser();
  var parsed = parser.parse(args);
  if (parsed['help']) {
    printUsage(parser);
    exit(0);
  }
  if (parsed.rest.isNotEmpty ||
      parsed['code'].isEmpty ||
      parsed['src'].isEmpty ||
      parsed['dst'] == null) {
    printUsage(parser);
    exit(1);
  }

  if (parsed['verbose']) {
    Logger.root.level = Level.FINEST;
  }

  fileTextWhiteList.addAll(parsed['textFileExt']);

  var snippets = parsed['code']
      .fold({}, (prev, code) => prev..addAll(extractSnippetsFromPath(code)));

  var dst = parsed['dst'];
  prepareDstDir(new Directory(dst), deleteFirst: parsed['deleteDst']);

  parsed['src'].forEach((path) {
    var mapper = parsed['reduceSrcPathForCopy']
        ? fileNameMapper(path, dst)
        : fileNameMapper('', dst);
    copyPathInjectingSnippets(path, mapper, snippets,
        removeSnippetAnnotations: parsed['cleanSrcFromAnnotations']);
  });

  exit(0);
}
