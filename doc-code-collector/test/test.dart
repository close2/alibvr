import 'dart:io';

main() {
  Process.run('dart', ['../bin/main.dart'], workingDirectory: 'test').then((ProcessResult pr) {
    print('Exit code: ${pr.exitCode}');
    print(pr.stdout);
    print(pr.stderr);
  });
}