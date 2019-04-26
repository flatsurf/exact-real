#!/bin/bash
set -eo pipefail

# Check that the tests actually pass before we attempt to profile them
make check

# We collect test results in an Airspeed Velocity database if we have the necessary credentials
if [ -n "$ASV_SECRET_KEY" ];then
  conda install --quiet -y git
  git config --global user.name 'CI Benchmark'
  git config --global user.email 'benchmark@ci.invalid'
  git config --global push.default nothing
  mkdir -p ~/.ssh
  sudo yum install -y openssh-clients
  ssh-keyscan -H github.com >> ~/.ssh/known_hosts

  set +x
  echo $ASV_SECRET_KEY | base64 -d > ~/.ssh/id_rsa
  chmod 400 ~/.ssh/id_rsa
  set -x

  cd /home/conda/feedstock_root

  # Clone performance data of previous runs
  rm -rf .asv/results
  git clone git@github.com:flatsurf/exact-real-asv.git .asv/results

  asv run
  
  pushd .asv/results
  git add .
  git commit -m "Added benchmark run"
  unset SSH_AUTH_SOCK
  git push origin master:master
  popd

  asv gh-pages --no-push
  git push git@github.com:flatsurf/exact-real-asv.git gh-pages:gh-pages -f
fi
