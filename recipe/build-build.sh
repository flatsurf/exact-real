#!/bin/bash
set -exo pipefail

./recipe/configure.sh

# Only pass if some static checks go through
LD_LIBRARY_PATH="$PREFIX/lib:$LD_LIBRARY_PATH" ./recipe/build-distcheck.sh
# We don't run static analysis at the moment. cppcheck does not fully support
# C++17, and clang-tidy is not packaged for conda-forge yet.
# ./recipe/build-clang-tidy.sh
./recipe/build-clang-format.sh
./recipe/build-todo.sh

./recipe/build_.sh
