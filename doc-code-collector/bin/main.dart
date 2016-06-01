import 'dart:io';
import 'package:mime/mime.dart' as mime;

/**
 * This script extracts code snippets from codeDir.
 * The snippets must start with a name enclosed in « and » optionally followed
 * by [x,y] → x being a regular expression and y it's replacement optionally
 * followed by by *\/
 * Everything after the » (or *\/) is the considered snippet code up to the
 * next /\*¤*\/.
 * The regular expression -- x to y -- replacement is then applied to the
 * extracted text.
 *
 * All files from docSources are then copied to docDest and all
 * +++snippetNames+++ are replaced by the snipped codes.
 * Optionally a regular expression may be added before the 2nd +++
 * +++snippetName[regFrom,regTo]+++
 *
 * The regular expression replacement is then applied to the snipped before
 * inserting it.
 */

const docDir = 'doc';

const codeDir = 'code';
const docSources = const ['doxygen', 'README.md'];
const docDest = 'build';

Map<String, String> snippets = {};

bool isFile(String path) {
  return FileSystemEntity.typeSync(path) == FileSystemEntityType.FILE;
}

String applyRegExp(String input, String regFrom, String regTo) {
  if (regFrom == null || regTo == null) {
    return input;
  }
  var r = new RegExp(regFrom);
  return input.split('\n').map((line) => line.replaceAll(r, regTo)).join('\n');
}

main(List<String> args) async {
  // Go recursively over all code files and build map of code snippets.
  var dir = new Directory('$docDir/$codeDir');

  var files = dir.list(recursive: true);

  const rRegFrom = r'([^,]+)';
  const rRegTo = r'([^\]]+)';
  const rReg = r'(\[' + rRegFrom + r'\,' + rRegTo + r'\])?';

  await files.where((fse) => isFile(fse.path)).forEach((fse) {
    String content = (fse as File).readAsStringSync();

    // Now find our start and stop characters.
    // Snippets always start with their name enclosed by « and »
    // optionally followed by a regular expression replacement part
    // optionally follwed by */
    // followed by the snippet itself until the end character: ¤
    // which may be enclosed in comments: /*¤*/
    // /*«Sn Name»*/uint8_t x = 3;/*¤*/
    // « or ¤*/ do not have to be at the beginning or end of a line.
    // Nested snippets are not yet supported.
    const rSnippetName = r'([^»\[]+)';
    const rEndComment = r'(\*/)?';
    const rCode = r'([^¤]*)';
    const rEnd = r'(¤[^*]|/\*¤\*/)';
    //  match        1              2 3 4         5             6       7
    const r = r'«' + rSnippetName + rReg + r'»' + rEndComment + rCode + rEnd;
    var regExp = new RegExp(r);
    var fileSnippets = regExp.allMatches(content);

    fileSnippets.forEach((match) {
      var name = match[1];
      var code = match[6];

      var regFrom = match[3];
      var regTo = match[4];
      code = applyRegExp(code, regFrom, regTo);
      snippets[name] = code;
    });
  });

  var d = new Directory('$docDir/$docDest');
  if (d.exists()) {
    d.deleteSync(recursive: true);
  }
  d.createSync();

  for (var docSource in docSources) {
    Iterable<FileSystemEntity> docFiles;
    if (FileSystemEntity.isFileSync('$docDir/$docSource')) {
      docFiles = [new File('$docDir/$docSource')];
    } else {
      var dirSrc = new Directory('$docDir/$docSource');
      docFiles = dirSrc
          .listSync(recursive: true, followLinks: false)
          .where((fse) => isFile(fse.path));
    }
    docFiles.forEach((File fse) {
      var outFileName = fse.path.substring(docDir.length + 1);
      var outFile = new File('${d.path}/$outFileName')
        ..createSync(recursive: true);

      var mimeType = mime.lookupMimeType(fse.path);
      if (mimeType != null && !mimeType.startsWith('text')) {
        var dest = outFile.path;
        // assume binary or image and simply copy
        outFile.deleteSync(); // create and delete so that recursive flag
        // creates the directory structure
        fse.copySync(dest);
        return;
      }

      String content = fse.readAsStringSync();

      // replace all occurences of +++SnippetName+++ with the snippet Code
      content = snippets.keys.fold(content, (String content, snippetName) {
        var r = r'\+\+\+' + snippetName + rReg + r'\+\+\+';
        var reg = new RegExp(r);
        return content.replaceAllMapped(reg, (m) {
          var sn = snippets[snippetName];
          return applyRegExp(sn, m[2], m[3]);
        });
      });

      // create outputFile:
      outFile.writeAsStringSync(content);
    });
  }
}
