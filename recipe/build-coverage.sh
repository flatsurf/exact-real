#!/bin/bash
set -e

# We build with gcov support enabled so we can report coverage statistics to
# coveralls.
export EXTRA_CXXFLAGS="--coverage"
./recipe/build-build.sh

# conda-build is going to remove our work directory. However, we need to
# restore it to make sense of the coverage files later.
tar cPf /tmp/artifacts.tar `pwd`
