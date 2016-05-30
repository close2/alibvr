#!/bin/bash

MKDOC=$0
P=$(dirname $MKDOC)
DOC_CODE_DIR="doc/code"

cd "$P"
cd "$DOC_CODE_DIR"
make clean

cd -

doxygen

dart doc-code-collector/bin/main.dart
cp doc/build/README.md .
