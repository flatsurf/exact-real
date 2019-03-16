#!/bin/bash
set -e

unset ASV_SECRET_KEY

# Restore metadata that gcov needs to make sense of the coverage results
tar xPf /tmp/artifacts.tar

make check

# Report coverage data to coveralls
conda install -y --quiet pip git gcc_linux-64
pip install pyyaml cpp-coveralls
cd `find $CONDA_BLD_PATH -name '*.gcno' | head -n1 | sed 's/\(.*work\).*/\1/g'`
set +x
if [ -n "$COVERALLS_REPO_TOKEN" ];then coveralls --gcov `which x86_64-conda_cos6-linux-gnu-gcov` --gcov-options '\-lrp' -b src; fi
