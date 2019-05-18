#!/bin/bash
set -e

# Enforce proper formatting of our source code
conda install -y --quiet clangdev=8.0.0
clang-format -i -style=file `git ls-files '*.cc' '*.hpp' '*.ipp' '*.h' '*.h.in' '.hpp.in'`
git diff --exit-code
