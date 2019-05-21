#!/bin/bash
set -exo pipefail

mkdir -p ~/.ssh

set +x
# We publish test results in an Airspeed Velocity database if we have the necessary credentials
# Note that ASV_SECRET_KEY must have been generated with ssh-keygen -t rsa -m pem
# otherwise the old ssh implementation used here won't be able to make sense of it
if echo "$ASV_SECRET_KEY" | base64 -d > ~/.ssh/id_rsa; then
  have_asv=yes
  chmod 400 ~/.ssh/id_rsa
fi
set -x

# We need setuptools until https://github.com/conda-forge/asv-feedstock/pull/10 has been resolved
conda install --quiet -y git asv setuptools
git config --global user.name 'CI Benchmark'
git config --global user.email 'benchmark@ci.invalid'
git config --global push.default nothing

sudo yum install -y openssh-clients
ssh-keyscan -H github.com >> ~/.ssh/known_hosts

pushd /home/conda/feedstock_root

# Clone performance data of previous runs
rm -rf .asv/results
if test "x$have_asv" = "xyes"; then
  git clone git@github.com:flatsurf/exact-real-asv.git .asv/results
else
  git clone https://github.com/flatsurf/exact-real-asv.git .asv/results
fi
  
cp recipe/asv-machine.json ~/.asv-machine.json
# We have to be on a branch. Otherwise, asv cannot find the branch for the
# commits and refuses to generate graphs.
git checkout -b master

asv run --machine=azure

pushd .asv/results
git add .
git commit -m "Added benchmark run"
unset SSH_AUTH_SOCK
if test "x$have_asv" = "xyes"; then
  git push origin master:master
fi
popd

asv gh-pages --no-push
if test "x$have_asv" = "xyes"; then
  # We cannot push to origin since the outer repository has been cloned with https://github.com/…
  git push git@github.com:flatsurf/exact-real-asv.git gh-pages:gh-pages -f
fi

popd
