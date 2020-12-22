export CXXFLAGS="$CXXFLAGS -g3"

cd pyexactreal

autoreconf -ivf

./configure --prefix="$PREFIX" --without-benchmark --without-pytest
make install
