#!/bin/bash
set -e

# Disable cppyy tests since we cannot install it in conda-forge yet
export DISTCHECK_CONFIGURE_FLAGS="--without-pytest"

# Make sure that our generated tarball is conforming to the standards.
make -j$CPU_COUNT distcheck
