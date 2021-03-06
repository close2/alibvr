import 'dart:io';

import 'package:logging/logging.dart';
import 'package:test/test.dart';

import 'package:doc_code_collector/snippets/snippets.dart' as sn;

main() {
  Logger.root.level = Level.FINEST;
  Logger.root.onRecord.listen((LogRecord rec) {
    print('${rec.level.name}: ${rec.time}: ${rec.message}');
  });

  Map<String, String> snippets;

  group('Extracting snippets', () {
    test('Extract snippets from path', () {
      snippets = sn.extractSnippetsFromPath('data');
      expect(snippets.length, greaterThan(0));
      expect(snippets['ENUM_C'], isNotNull);
      expect(snippets['REC'], equals('abc'));
    });
    test('Extract snippet from file with regexp', () {
      var snippets = sn.extractSnippetsFromFile(new File('data/ports.h'));
      expect(snippets['ENUM_C'], equals('\nC'));
    });
  });

  test('FileNameMapper only keeps last path element of src', () {
    var mapper = sn.fileNameMapper('src/with/some/path/elements', 'dst');
    expect(mapper('src/with/some/path/elements/file1'), equals('dst/elements/file1'));
  });

  group('Injecting snippets', () {
    test('Inject snippet with regexp', () {
      var contentWSnippet = sn.injectSnippets('012+++ENUM_C[^,AB]+++34', snippets);
      expect(contentWSnippet, equals('012AB\nABC34'));
    });
  });
}