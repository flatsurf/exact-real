#!/bin/bash
set -ex

source $RECIPE_DIR/environment.sh

cd pyexactreal
$SNIPPETS_DIR/make/install.sh
