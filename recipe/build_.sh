#!/bin/bash
set -exo pipefail

make -j$CPU_COUNT CXXFLAGS="$CXXFLAGS $EXTRA_CXXFLAGS"
