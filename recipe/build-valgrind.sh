#!/bin/bash
set -e

conda install -y valgrind

./recipe/configure.sh
./recipe/build_.sh
