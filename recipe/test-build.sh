#!/bin/bash
set -eo pipefail

# Check that the tests actually pass before we attempt to profile them
make check || (cat `find -name test-suite.log`; false)

./recipe/test-asv.sh

# Finally some static checks
LD_LIBRARY_PATH="$PREFIX/lib:$LD_LIBRARY_PATH" ./recipe/build-distcheck.sh
# We don't run static analysis at the moment. cppcheck does not fully support
# C++17, and clang-tidy is not packaged for conda-forge yet.
# ./recipe/build-clang-tidy.sh
./recipe/build-clang-format.sh
./recipe/build-todo.sh
