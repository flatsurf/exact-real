#!/bin/bash
set -exo pipefail

# Create Makefiles
./bootstrap
./configure --prefix="$PREFIX" CXXFLAGS="$CXXFLAGS" CXX=$CXX LDFLAGS="-L$PREFIX/lib -Wl,-rpath-link=$PREFIX/lib $LDFLAGS" CPPFLAGS="-isystem $PREFIX/include $CPPFLAGS" || (cat config.log; exit 1)

# Only pass if some static checks go through
LD_LIBRARY_PATH="$PREFIX/lib:$LD_LIBRARY_PATH" ./recipe/build-distcheck.sh
# We don't run static analysis at the moment. cppcheck does not fully support
# C++17, and clang-tidy is not packaged for conda-forge yet.
# ./recipe/build-clang-tidy.sh
./recipe/build-clang-format.sh
./recipe/build-todo.sh

# Called from build.sh to build and install the normal version of this package.
make -j$CPU_COUNT CXXFLAGS="$CXXFLAGS $EXTRA_CXXFLAGS"
