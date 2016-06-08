import 'dart:io';

const String snippetsProg = 'doc-code-collector/bin/snippets.dart';

main() {
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

  // Make sure our documentation code really compiles:
  var makeExitCodes = makes.map((makePath) => Process.runSync('make', [], workingDirectory: makePath).exitCode);
  if (makeExitCodes.any((c) => c != 0)) {
    print('Error while building the doc codes (with make)');
    exit(9);
  }

  // Extract and inject snippets.

  // We also need to do this for our src code, because we have to inject the
  // snippets before executing doxygen.  Our src directory is therefore both
  // a code directory and a doc directory!
  var dstArgs = ['-d', dst];
  var codeArgs = codes.fold([], (prev, code) => prev.addAll(['-c', code]));
  var docArgs = docs.fold([], (prev, docSrc) => prev.addAll(['-s', docSrc]));

  var args = [snippetsProg]..addAll(dstArgs)..addAll(codeArgs)..addAll(docArgs);
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
  doxyFileF.copySync('$dst/DoxyFile');

  var srcWSnDir = new Directory('$dst/$src');
  if (!srcWSnDir.existsSync()) {
    print('Can\'t find $dst/$src which should contain the src code for '
    'doxygen with injected snippets');
    print('This shouldn\'t happen!');
    exit(3);
  }
  var exitCodeDoxygen = Process.runSync('doxygen', [], workingDirectory: '$dst');
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
  readmeF.copySync(readme);
}
