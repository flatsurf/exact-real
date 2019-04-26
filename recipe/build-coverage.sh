#!/bin/bash
set -e

# We build with gcov support enabled so we can report coverage statistics to
# coveralls.
export EXTRA_CXXFLAGS="--coverage"
./recipe/configure.sh
./recipe/build_.sh
