#!/bin/bash
set -e

conda install -y --quiet cppcheck

case `uname` in
    Darwin)
        # conda-forge's cppcheck does not work on macOS, see https://github.com/flatsurf/flatsurf/pull/22
        ;;
    *)
        cppcheck --enable=all --error-exitcode=1 --inline-suppr -j$CPU_COUNT src
        ;;
esac
