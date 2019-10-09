#!/bin/bash
set -ex

source $RECIPE_DIR/environment.sh

cd libexactreal
$SNIPPETS_DIR/make/install.sh
