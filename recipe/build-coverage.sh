#!/bin/bash
set -ex

# Run ./configure
source ./recipe/cflags.sh
./recipe/configure.sh

# Build libexactreal
export EXTRA_CXXFLAGS="--coverage"
make CXXFLAGS="$CXXFLAGS $EXTRA_CXXFLAGS"

# Run all our test suites
make check

# /tmp/secrets contains some CI-injected credentials to services such as
# coveralls or ASV.
source /tmp/secrets || true

# Send coverage data to coveralls.io
./recipe/coverage.sh
