#!/bin/bash
set -e

unset ASV_SECRET_KEY

make check-valgrind
