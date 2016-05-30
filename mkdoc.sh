#!/bin/bash

MKDOC=$0
P=$(dirname $MKDOC)

dart $P/doc-code-collector/bin/main.dart
cp doc/build/README.md .
