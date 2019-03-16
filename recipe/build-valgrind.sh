#!/bin/bash
set -ex

# Running our entire test suite with valgrind enabled might take too long,
# so we run everything with the lest intrusive lmcheck and only some select
# tests with valgrind.
export EXTRA_CXXFLAGS="-lmcheck"
./recipe/build-build.sh
