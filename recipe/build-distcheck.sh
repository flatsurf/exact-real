#!/bin/bash
set -e

# Make sure that our generated tarball is conforming to the standards.
make -j$CPU_COUNT distcheck
