#!/bin/bash

MKDOC=$0
P=$(dirname $MKDOC)

cd "$P"

doxygen

dart doc-code-collector/bin/main.dart
cp doc/build/README.md .
