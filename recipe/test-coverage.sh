#!/bin/bash
set -exo pipefail

unset ASV_SECRET_KEY

make check

# Report coverage data to coveralls
conda install -y --quiet pip git gcc_linux-64
# Allow installation of cpp-coveralls with pip (otherwise disabled during conda builds)
unset PIP_NO_INDEX
unset PIP_NO_DEPENDENCIES
pip install cpp-coveralls

cd $SRC_DIR/src/libexactreal/
set +x
if [ ${#COVERALLS_REPO_TOKEN} = 33 ];then coveralls --gcov `which x86_64-conda_cos6-linux-gnu-gcov` --exclude exact-real/external --gcov-options '\-lrp' -b .; fi
