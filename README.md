![Test](https://github.com/flatsurf/exact-real/workflows/Test/badge.svg)
[![codecov](https://codecov.io/gh/flatsurf/exact-real/branch/master/graph/badge.svg)](https://codecov.io/gh/flatsurf/exact-real)
[![asv](http://img.shields.io/badge/benchmarked%20by-asv-blue.svg?style=flat)](https://flatsurf.github.io/exact-real/asv/)

## Current Release Info

We release this package regularly with [rever](https://regro.github.io/rever-docs/index.html); typically with every push to the master branch.

This repository contains two related projects:

* **libexactreal** a C++ library
* **pyexactreal** a Python wrapper for **libexactreal**

| Name | Downloads | Version | Platforms |
| --- | --- | --- | --- |
| [![Build](https://img.shields.io/badge/recipe-libexactreal-green.svg)](https://anaconda.org/flatsurf/libexactreal) | [![Conda Downloads](https://img.shields.io/conda/dn/flatsurf/libexactreal.svg)](https://anaconda.org/flatsurf/libexactreal) | [![Conda Version](https://img.shields.io/conda/vn/flatsurf/libexactreal.svg)](https://anaconda.org/flatsurf/libexactreal) | [![Conda Platforms](https://img.shields.io/conda/pn/flatsurf/libexactreal.svg)](https://anaconda.org/flatsurf/libexactreal) |
| [![Build](https://img.shields.io/badge/recipe-pyexactreal-green.svg)](https://anaconda.org/flatsurf/pyexactreal) | [![Conda Downloads](https://img.shields.io/conda/dn/flatsurf/pyexactreal.svg)](https://anaconda.org/flatsurf/pyexactreal) | [![Conda Version](https://img.shields.io/conda/vn/flatsurf/pyexactreal.svg)](https://anaconda.org/flatsurf/pyexactreal) | [![Conda Platforms](https://img.shields.io/conda/pn/flatsurf/pyexactreal.svg)](https://anaconda.org/flatsurf/pyexactreal) |

## Install with Conda

You can install this package with conda. Download and install [Miniconda](https://conda.io/miniconda.html), then run

```
conda config --add channels conda-forge
conda create -n exactreal -c flatsurf libexactreal pyexactreal
conda activate exactreal
```

## Run with binder in the Cloud

You can try out the projects in this repository in a very limited environment online by clicking the following links:

* **libexactreal** [![Binder](https://mybinder.org/badge_logo.svg)](https://mybinder.org/v2/gh/flatsurf/exact-real/master?filepath=doc%2Fbinder%2FSample.ipynb)
* **pyexactreal** [![Binder](https://mybinder.org/badge_logo.svg)](https://mybinder.org/v2/gh/flatsurf/exact-real/master?filepath=doc%2Fbinder%2FSample.pyexactreal.ipynb)

## Build from the Source Code Repository

We are following a standard autoconf setup, i.e., you can create the library
`libexactreal/src` with the following:

```
git clone --recurse-submodules https://github.com/flatsurf/exact-real.git
cd exact-real
./bootstrap
./configure
make
make check # to run our test suite
make install # to install into /usr/local
```

For best performance run `CXXFLAGS="-O3 -flto -march=native -mtune=native"
CXX="g++ -fno-semantic-interposition" ./configure` instead of `./configure` as
this code greatly benefits from flto inlining. (Unfortunately, libtool filters
out `-fno-semantic-interposition` as of early 2019 so we can not pass it as
part of `CXXFLAGS`. If you are using clang, `-fno-semantic-interposition` does
not seem to be necessary.) Do not use `-Ofast` or `-ffast-math` as parts of our
code rely on IEEE compliance. You might want to add `-g3` to the `CXXFLAGS`
which does not hurt performance but gives a better debugging experience. For
the best debugging experience, you might want to replace `-O3` with `-Og` or
even `-O0` but the latter results in very poor performance.

If your compiler supports it, you can try to add `-fvisibility=hidden
-fvisibility-inlines-hidden` to your `CXXFLAGS`. This hides internal bits in
the resulting library which have lead to crashes in the past due to conflicting
header-only libraries.

If your linker supports it, you should use `./configure --with-version-script`
to shrink the resulting shared library to an exact curated list of versioned
symbols.

Additionally, you might want to run configure with ` --disable-static` which
improves the build time.

[perf](https://perf.wiki.kernel.org/index.php/Main_Page) works well to profile
when you make sure that `CXXFLAGS` contains `-fno-omit-framepointer`. You can
then for example run our test suite with `perf record --call-graph dwarf make
check`. Apart from perf itself there are several ways to analyze the output,
[hotspot](https://github.com/KDAB/hotspot) might be the most convenient one at
the time of this writing.


## Build from the Source Code Repository with Conda Dependencies

To build this package, you need a fairly recent C++ compiler and probably some
packages that might not be readily available on your system. If you don't want
to use your distribution's packages, you can provide these dependencies with
conda. Download and install [Miniconda](https://conda.io/miniconda.html), then
run

```
git clone --recurse-submodules https://github.com/flatsurf/exact-real.git
cd exact-real

conda create -n exactreal-build ccache
conda env update -n exactreal-build -f libexactreal/environment.yml
conda env update -n exactreal-build -f pyeaxctreal/environment.yml
conda activate exactreal-build

./bootstrap
./configure --prefix="$CONDA_PREFIX"
make
```

## Build from the Source Code Repository with Conda

The conda recipe in `recipe/` is built automatically as part of our Continuous
Integration. If you want to build the recipe manually, something like the
following should work:

```
git clone --recurse-submodules https://github.com/flatsurf/exact-real.git
cd exact-real
conda activate root
conda config --add channels conda-forge
conda config --add channels flatsurf # if you want to pull in the latest version of dependencies
conda install conda-build conda-forge-ci-setup=2
export FEEDSTOCK_ROOT=`pwd`
export RECIPE_ROOT=${FEEDSTOCK_ROOT}/recipe
export CI_SUPPORT=${FEEDSTOCK_ROOT}/.ci_support
export CONFIG=linux_
make_build_number "${FEEDSTOCK_ROOT}" "${RECIPE_ROOT}" "${CI_SUPPORT}/${CONFIG}.yaml"
conda build "${RECIPE_ROOT}" -m "${CI_SUPPORT}/${CONFIG}.yaml" --clobber-file "${CI_SUPPORT}/clobber_${CONFIG}.yaml"
```

You can then try out the package that you just built with:
```
conda create -n exact-real-test --use-local libexactreal
conda activate exact-real-test
```

## Run Tests and Benchmark

`make check` runs all tests and benchmarks. During development `make check TESTS=module`
only runs the tests for `module`. For randomized tests, you might want to add
`GTEST_REPEAT=1024` to run such tests repeatedly. Note that the environment
variable `EXACTREAL_CHECK` is passed on to the tests and benchmarks, i.e., you
could add `EXACTREAL_CHECK="--benchmark_min_time=.02"` to not let the
benchmarks run as long as they would usually.

## Maintainers

* [@saraedum](https://github.com/saraedum)
* [@videlec](https://github.com/videlec)
