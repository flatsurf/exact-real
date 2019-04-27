#!/bin/bash
set -exo pipefail

./recipe/configure.sh
./recipe/build_.sh
