#!/bin/bash
set -e

conda install -y valgrind

./recipe/build-build.sh
