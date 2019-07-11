#!/bin/bash
set -exo pipefail

# Create coverage statistics
make check

# Install gcov
conda install -y --quiet gcc_linux-64

cd $SRC_DIR/libexactreal/src

git remote remove origin || true
git remote add origin https://github.com/flatsurf/flatsurf.git
git fetch origin
git checkout -b master
git branch -u origin/master

set +x
if [ ${#COVERALLS_REPO_TOKEN} = 36 ];then
  bash <(curl -s https://codecov.io/bash) -t $COVERALLS_REPO_TOKEN -x `which x86_64-conda_cos6-linux-gnu-gcov` -a '\-lrp' -p .
fi
