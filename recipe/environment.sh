#!/bin/bash

# ##################################################################
# Source this file to setup additional environment variables for the
# conda-build environment.
# ##################################################################

export SNIPPETS_DIR=${SNIPPETS_DIR:-$SRC_DIR/recipe/snippets}
export GITHUB_ORGANIZATION=flatsurf
export GITHUB_REPOSITORY=exact-real
export build_flavour=${build_flavour:-release}

source $SNIPPETS_DIR/cxx/environment.sh
source $SNIPPETS_DIR/make/environment.sh
source $SNIPPETS_DIR/asv/environment.sh
source $SNIPPETS_DIR/codecov/environment.sh

export SUBDIRS=libexactreal:pyexactreal
