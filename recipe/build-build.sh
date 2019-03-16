#!/bin/bash
set -exo pipefail

# Create Makefiles
./bootstrap
./configure --prefix="$PREFIX" CXXFLAGS="$CXXFLAGS" CXX=$CXX || (cat config.log; exit 1)

# Only pass if some static checks go through
./recipe/build-distcheck.sh
./recipe/build-cppcheck.sh
./recipe/build-clang-format.sh
./recipe/build-todo.sh

# Called from build.sh to build and install the normal version of this package.
make -j$CPU_COUNT CXXFLAGS="$CXXFLAGS $EXTRA_CXXFLAGS"
make install
