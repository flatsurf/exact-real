#!/bin/bash
set -ex

source $RECIPE_DIR/environment.sh

$SNIPPETS_DIR/autoconf/run.sh
$SNIPPETS_DIR/make/run.sh

$SNIPPETS_DIR/clang-format/run.sh
$SNIPPETS_DIR/asv/run.sh
$SNIPPETS_DIR/todo/run.sh
$SNIPPETS_DIR/codecov/run.sh
