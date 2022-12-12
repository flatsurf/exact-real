export CXXFLAGS="$CXXFLAGS -g3"

cd libexactreal

autoreconf -ivf

./configure --prefix="$PREFIX" --without-benchmark --with-version-script --disable-static
make install
