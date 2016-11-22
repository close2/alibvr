import 'dart:io';
import 'package:args/args.dart';

const defaultNameGenerations = 30;

main(List<String> args) {
  final parser = new ArgParser();
  parser.addOption('part', help: 'Which part should be generated.', allowed: ['TaskList', 'TaskName'], defaultsTo: 'TaskList');
  parser.addOption('count',
      abbr: 'n', help: 'Number of allowed name generations.', defaultsTo: '$defaultNameGenerations');
  parser.addOption('file',
      abbr: 'o', help: 'Store output in this file.');
  parser.addFlag('help', abbr: 'h', help: 'Print this help.', defaultsTo: false, negatable: false);

  final parseRes = parser.parse(args);

  if (parseRes['help']) {
    print(parser.usage);
    exit(0);
  }

  var c = int.parse(parseRes['count']);
  var out = parseRes['file'];

  var output = '';
  for (int i = c; i > 0; i--) {
    if (i == c) {
      output += '#if defined _TASK_LIST_COUNTER_$i\n';
    } else if (i > 1) {
      output += '#elif defined _TASK_LIST_COUNTER_$i\n';
    } else {
      output += '#else\n';
    }
    if (parseRes['part'] == 'TaskList') {
      output += '#  undef TASK_LIST\n';
      output += '#  define TL TaskList$i\n';
      output += '#  define TASK_LIST _tasks::TaskList$i\n';
    } else if (parseRes['part'] == 'TaskName') {
      output += '#  undef TASK_NAME\n';
      output += '#  define TASK_NAME TaskName$i\n';
    }
    output += '#  define _TASK_LIST_COUNTER_${i + 1}\n';
    if (i == 1) {
      output += '#endif';
    }
  }

  if (out == null) {
    print(output);
  } else {
    new File(out).writeAsStringSync(output);
  }
}


