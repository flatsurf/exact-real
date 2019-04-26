#!/bin/bash
set -eo pipefail

# Check that the tests actually pass before we attempt to profile them
make check

# We collect test results in an Airspeed Velocity database if we have the necessary credentials
if [ -n "$ASV_SECRET_KEY" ];then
  # We need setuptools until https://github.com/conda-forge/asv-feedstock/pull/10 has been resolved
  conda install --quiet -y git asv setuptools
  git config --global user.name 'CI Benchmark'
  git config --global user.email 'benchmark@ci.invalid'
  git config --global push.default nothing
  mkdir -p ~/.ssh
  sudo yum install -y openssh-clients
  ssh-keyscan -H github.com >> ~/.ssh/known_hosts

  set +x
  # Note that ASV_SECRET_KEY must have been generated with ssh-keygen -t rsa -m pem
  # otherwise the old ssh implementation used here won't be able to make sense of it
  echo $ASV_SECRET_KEY | base64 -d > ~/.ssh/id_rsa
  chmod 400 ~/.ssh/id_rsa
  set -x

  pushd /home/conda/feedstock_root

  # Clone performance data of previous runs
  rm -rf .asv/results
  git clone git@github.com:flatsurf/exact-real-asv.git .asv/results

  cp recipe/asv-machine.json ~/.asv-machine.json
  git checkout -b master

  asv run --machine=azure
  
  pushd .asv/results
  git add .
  git commit -m "Added benchmark run"
  unset SSH_AUTH_SOCK
  git push origin master:master
  popd

  asv gh-pages --no-push
  git push git@github.com:flatsurf/exact-real-asv.git gh-pages:gh-pages -f

  popd
fi

# Finally some static checks
LD_LIBRARY_PATH="$PREFIX/lib:$LD_LIBRARY_PATH" ./recipe/build-distcheck.sh
# We don't run static analysis at the moment. cppcheck does not fully support
# C++17, and clang-tidy is not packaged for conda-forge yet.
# ./recipe/build-clang-tidy.sh
./recipe/build-clang-format.sh
./recipe/build-todo.sh
