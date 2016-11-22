#!/bin/bash

DART_DOC_PROG="tools/doc-code-collector/bin/build-alibvr-doc.dart"

MKDOC=$0
P=$(dirname $MKDOC)
cd "$P"

dart "$DART_DOC_PROG"

