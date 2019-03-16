#!/bin/bash
set -e

# Enforce proper formatting of our source code
conda install -y --quiet clangdev
clang-format -i --style=Google `ls src/libexactreal/*.cc src/libexactreal/exact-real/*.hpp | grep -v exact-real.hpp`
git diff --exit-code
