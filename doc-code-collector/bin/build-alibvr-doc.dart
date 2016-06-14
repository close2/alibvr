import 'dart:io';

import 'package:logging/logging.dart';

const String snippetsProg = 'doc-code-collector/bin/snippets.dart';

Logger log = new Logger('build-alibvr-doc');

main() {
  Logger.root.level = Level.FINEST;
  Logger.root.onRecord.listen((LogRecord rec) {
    print('${rec.level.name}: ${rec.time}: ${rec.message}');
  });

  var snippetsProgF = new File(snippetsProg);
  if (!snippetsProgF.existsSync()) {
    print('Can\'t find snippets program.  '
        'Maybe you need to change the working directory\n'
        'Program: $snippetsProg');
    exit(1);
  }

  const src = 'src';
  const readme = 'README.md';

  const makes = const ['doc/code'];

  const dst = 'doc/build';
  const codes = const [src, 'doc/code'];
  const docs = const [src, 'doc/$readme'];

  // relative from dst
  const doxygenOutRel = '../../gh-pages/doxygen';

  // Make sure our documentation code really compiles:
  makes.forEach((makePath) {
    log.finer('Running make in $makePath');
    var makeExitCode =
        Process.runSync('make', [], workingDirectory: makePath).exitCode;
    if (makeExitCode != 0) {
      log.warning(
          'Running make in $makePath did not exit with 0, but $makeExitCode');
      exit(9);
    }
  });

  // Delete old dst
  var dstDir = new Directory(dst);
  if (dstDir.existsSync()) {
    dstDir.deleteSync(recursive: true);
  }

  // Extract and inject snippets.

  // We also need to do this for our src code, because we have to inject the
  // snippets before executing doxygen.  Our src directory is therefore both
  // a code directory and a doc directory!
  var dstArgs = ['-d', dst];
  var codeArgs = codes.fold([], (prev, code) => prev..addAll(['-c', code]));
  var docArgs = docs.fold([], (prev, docSrc) => prev..addAll(['-s', docSrc]));
  var otherArgs = ['-c'];

  var args = [snippetsProg]
    ..addAll(dstArgs)
    ..addAll(codeArgs)
    ..addAll(docArgs)
    ..addAll(otherArgs);
  log.finer('Executing dart with $args as arguments');
  var exitCodeSnippets = Process.runSync('dart', args).exitCode;
  if (exitCodeSnippets != 0) {
    print('Something went wrong while executing $snippetsProg');
    exit(100 + exitCodeSnippets);
  }

  // Run doxygen. (Copy Doxyfile first)
  var doxyFileF = new File('Doxyfile');
  if (!doxyFileF.existsSync()) {
    print('Can\'t find Doxyfile');
    exit(2);
  }
  log.finer('Copying Doxyfile to $dst/Doxyfile');
  var doxyfileDstF = new File('$dst/Doxyfile');
  doxyfileDstF.writeAsStringSync(doxyFileF.readAsStringSync().replaceFirst(
      new RegExp(r'\nOUTPUT_DIRECTORY.*\n'),
      '\nOUTPUT_DIRECTORY = $doxygenOutRel\n'));

  var srcWSnDir = new Directory('$dst/$src');
  if (!srcWSnDir.existsSync()) {
    print('Can\'t find $dst/$src which should contain the src code for '
        'doxygen with injected snippets');
    print('This shouldn\'t happen!');
    exit(3);
  }

  log.finer('Executing doxygen in $dst');
  var exitCodeDoxygen =
      Process.runSync('doxygen', [], workingDirectory: '$dst').exitCode;
  if (exitCodeDoxygen != 0) {
    print('Something went wrong while executing doxygen.');
    exit(100 + exitCodeDoxygen);
  }

  // Copy README.md
  var readmeF = new File('$dst/$readme');
  if (!readmeF.existsSync()) {
    print('Can\'t find $dst/$readme.  This shouldn\'t happen.');
    exit(4);
  }
  log.finer('Copying $dst/$readme to $readme');
  readmeF.copySync(readme);
}
