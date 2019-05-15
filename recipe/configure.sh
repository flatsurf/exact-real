#!/bin/bash
set -exo pipefail

if [ -z $PREFIX ]; then
        export PREFIX="$ASV_ENV_DIR"
fi

if [ -z $PREFIX ]; then
        echo '$PREFIX must be set. Aborting.'
        false
fi

# Create Makefiles
./bootstrap
# The Python tests need cppyy. The one on pypi needs glibc >= 2.15 which this
# RedHat machine does not provide. We should build cppyy on conda-forge so that
# this requirements goes away.
./configure --without-pytest --prefix="$PREFIX" CXXFLAGS="$CXXFLAGS" CXX=$CXX LDFLAGS="-L$PREFIX/lib -Wl,-rpath-link=$PREFIX/lib $LDFLAGS" CPPFLAGS="-isystem $PREFIX/include $CPPFLAGS" || (cat config.log; exit 1)
