library snippets;

import 'dart:io';
import 'package:logging/logging.dart';

Logger log = new Logger('snippets');

var fileTextWhiteList = ['.c', '.h', '.cpp', '.md', '.txt', '.html', '.S'];

const rSnippetName = r'([^»\[]+)';
const rEndComment = r'( ?\*/)?';
const rOptionalEol = r'\n?';
// Some trickery to get snippets to loose the first and last eol
// together with *.
// See the port defintions of README.md and ports.h for an example of
// complicated eols.
const rCode = r'([^¤]*[^¤\n \*])';
const rOptionalEolWs = r'\n? *\*? *';
const rEnd = r'(¤[^*]|¤$|/\*¤\*/|// *¤)';
const rRegFrom = r'([^,]+)';
const rRegTo = r'([^\]]*)';
const rReg = r'(\[' + rRegFrom + r'\,' + rRegTo + r'\])?';
const rSnippet = r'«' + // match:
    rSnippetName + // 1
    rReg + // 2 3 4
    r'»' +
    rEndComment + // 5
    rOptionalEol +
    rCode + // 6
    rOptionalEolWs +
    rEnd; // 7
const int snippetNameIndex = 1;
const int regFromIndex = 3;
const int regToIndex = 4;
const int codeIndex = 6;

bool isFile(String path) {
  return FileSystemEntity.typeSync(path) == FileSystemEntityType.FILE;
}

bool isTextFile(String path) {
  return fileTextWhiteList.any((ending) => path.endsWith(ending));
}

String applyRegExp(String input, String regFrom, String regTo) {
  log.finest('Applying RegExp «$regFrom» to «$regTo» on «$input»');

  if (regFrom == null || regTo == null) {
    return input;
  }
  var r = new RegExp(regFrom);
  return input.split('\n').map((line) => line.replaceAll(r, regTo)).join('\n');
}

Map<String, String> extractSnippetsFromString(String content) {
  // Now find our start and stop characters.
  // Snippets always start with their name enclosed by « and »
  // optionally followed by a regular expression replacement part
  // optionally follwed by */
  // followed by the snippet itself until the end character: ¤
  // which may be enclosed in comments: /*¤*/
  // /*«Sn Name»*/uint8_t x = 3;/*¤*/
  // « or ¤*/ do not have to be at the beginning or end of a line.
  // Nested snippets are not yet supported.
  var regExp = new RegExp(rSnippet);
  var fileSnippets = regExp.allMatches(content);

  var snippets = <String, String>{};
  fileSnippets.forEach((match) {
    var name = match[snippetNameIndex];
    var code = match[codeIndex];

    var regFrom = match[regFromIndex];
    var regTo = match[regToIndex];
    code = applyRegExp(code, regFrom, regTo);
    log.fine('Found snippet «$name»');
    log.finer('  «$name»: «$code»');
    snippets[name] = code;
  });
  return snippets;
}

Map<String, String> extractSnippetsFromFile(File file) {
  log.fine('Trying to extract snippets from file: «${file.path}»');
  String content = file.readAsStringSync();
  return extractSnippetsFromString(content);
}

Map<String, String> extractSnippetsFromDirRec(Directory dir) {
  log.fine('Trying to extract snippets from directory: «${dir.path}»');
  var files = dir.listSync(recursive: true);

  return files
      .where((fse) => isFile(fse.path))
      .where((fse) => isTextFile(fse.path))
      .fold(
          {},
          (Map<String, String> prev, fse) =>
              prev..addAll(extractSnippetsFromFile(fse)));
}

Map<String, String> extractSnippetsFromPath(String path) {
  if (isFile(path)) {
    return extractSnippetsFromFile(new File(path));
  } else {
    return extractSnippetsFromDirRec(new Directory(path));
  }
}

String injectSnippets(String content, Map<String, String> snippets) {
  log.fine('Trying to inject snippets');
  log.finest('  content: $content');
  // replace all occurences of +++SnippetName+++ with the snippet Code
  return snippets.keys.fold(content, (String content, snippetName) {
    var r = r'\+\+\+' + snippetName + rReg + r'\+\+\+';
    var reg = new RegExp(r);
    return content.replaceAllMapped(reg, (m) {
      log.fine('Replacing $snippetName');
      var sn = snippets[snippetName];
      var snRegExp = applyRegExp(sn, m[2], m[3]);
      log.finest('  with $snRegExp');
      return snRegExp;
    });
  });
}

String rmSnippetAnnotation(String content) {
  return content.replaceAllMapped(rSnippet, (m) => m[codeIndex]);
}

void copyInjectingSnippets(File from, File to, Map<String, String> snippets,
    {removeSnippetAnnotations: false}) {
  log.fine('Copy while injecting snippets file «${from.path}» to «${to.path}»');
  to.createSync(recursive: true);

  if (!isTextFile(from.path)) {
    log.fine(
        '«${from.path}» is not of type text.  Simply copying to «${to.path}»');
    var dest = to.path;
    // assume binary or image and simply copy
    to.deleteSync(); // create and delete so that recursive flag
    // creates the directory structure
    from.copySync(dest);
    return;
  }

  var content = from.readAsStringSync();
  content = injectSnippets(content, snippets);

  if (removeSnippetAnnotations) {
    content = rmSnippetAnnotation(content);
  }

  // create outputFile:
  to.writeAsStringSync(content);
}

void prepareDstDir(Directory dst, {deleteFirst: true}) {
  if (deleteFirst && dst.existsSync()) {
    log.fine('Deleting «${dst.path}»');
    dst.deleteSync(recursive: true);
  }

  log.fine('Creating «${dst.path}»');
  dst.createSync(recursive: true);
}

typedef String _mapF(String);

void copyPathInjectingSnippets(
    String path, _mapF srcDstMapFunction, Map<String, String> snippets,
    {removeSnippetAnnotations: false}) {
  log.fine('Copy file ${path} if file or recursively all files in ${path} '
      'if dir while injecting snippets using a '
      'src-to-dst map function');

  Iterable<File> files;
  if (isFile(path)) {
    files = [new File(path)];
  } else {
    var dir = new Directory(path);
    files = dir
        .listSync(recursive: true, followLinks: false)
        .where((fse) => isFile(fse.path));
  }

  files.forEach((File f) {
    var dstF = new File(srcDstMapFunction(f.path));
    copyInjectingSnippets(f, dstF, snippets,
        removeSnippetAnnotations: removeSnippetAnnotations);
  });
}

_mapF fileNameMapper(String srcPrefix, String addPrefix) {
  var rmPrefix =
      srcPrefix.replaceAllMapped(new RegExp(r'((.*/)*)(.+)'), (m) => m[1]);
  log.finer('srcPrefix: $srcPrefix; will remove $rmPrefix from files');
  return (String path) {
    log.finest('Stripping $rmPrefix from $path and adding $addPrefix');
    return '$addPrefix/${path.substring(rmPrefix.length)}';
  };
}
