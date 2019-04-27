dnl Process this file with autoconf to produce a configure script.

dnl We need at least autoconf 2.69 for this configure.ac to work.
AC_PREREQ([2.69])
AC_INIT([exact-real], [0.0.1], [julian.rueth@fsfe.org])

dnl Version number of the library c:r:a.
dnl Set to c+1:0:0 on a non-backwards-compatible change.
dnl Set to c+1:0:a+1 on a backwards-compatible change.
dnl Set to c:r+1:a if the interface is unchanged.
AC_SUBST([libexactreal_version_info], [0:0:0])

AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE([1.14.1])

dnl Find C++ Compiler
AC_PROG_CXX

dnl Set INSTALL or use install-sh.
AC_PROG_INSTALL
dnl We build our library with libtool.
LT_INIT

AC_CANONICAL_HOST

dnl We run the valgrind test-suite in make check-valgrind (if valgrind is installed)
AX_VALGRIND_DFLT([sgcheck], [off])
AX_VALGRIND_DFLT([drd], [off])
AX_VALGRIND_DFLT([helgrind], [off])
AX_VALGRIND_CHECK()

dnl Check for required libraries
AC_LANG([C++])

dnl We provide a Python module powered by cppyy
AC_ARG_WITH([python], AS_HELP_STRING([--without-python], [Do not build Python module]))
AS_IF([test "x$with_python" != "xno"],
      [AM_PATH_PYTHON([3.6], [have_python=yes], [have_python=no])],
      [have_python=no])
AS_IF([test "x$have_python" = "xyes"],
      [],
      [AS_IF([test "x$with_python" = "xyes"], [AC_MSG_ERROR([Python package requested but Python not found])])])
AM_CONDITIONAL([HAVE_PYTHON], [test "x$have_python" = "xyes"])

dnl We use some C++17 features, such as if constexpr
AX_CXX_COMPILE_STDCXX(17)

ACX_PTHREAD([], [AC_MSG_ERROR([pthread not supported])])

AC_CHECK_HEADERS([arb.h], , AC_MSG_ERROR([Arb headers not found]))
AC_CHECK_LIB([arb], [arb_init], , AC_MSG_ERROR([Arb library not found]), [-lgmp -lmpfr -lflint])

AC_CHECK_HEADERS([gmpxx.h], , AC_MSG_ERROR([GMP headers for C++ not found]))
dnl GMPXX does not contain anything that we can check for with AX_CXX_CHECK_LIB
dnl so we just check for something from the standard library, i.e., that -lgmpxx
dnl goes through
AX_CXX_CHECK_LIB([gmpxx], [std::runtime_error::what () const], , AC_MSG_ERROR([GMP library for C++ not found]), [-lgmp -lmpfr -lflint])

AC_CHECK_HEADERS([e-antic/renfxx.h], , AC_MSG_ERROR([E-ANTIC headers not found]))
AC_CHECK_LIB([eantic], [renf_elem_clear], , AC_MSG_ERROR([E-ANTIC library not found]), [-lgmp -lmpfr -lflint])
AX_CXX_CHECK_LIB([eanticxx], [eantic::renf_elem_class], , AC_MSG_ERROR([E-ANTIC library not found]), [-lgmp -lmpfr -lflint])

dnl Our test suite uses googletest and Google's C++ benchmark library.
dnl We fail if they cannot be found but let the user disable all checks explicitly.
AC_ARG_WITH([googletest], AS_HELP_STRING([--without-googletest], [Do not build C++ tests that require googletest/benchmark]))
AS_IF([test "x$with_googletest" != "xno"],
      [
       with_googletest=yes
       AC_CHECK_HEADERS([gtest/gtest.h], , AC_MSG_ERROR([googletest headers for make check not found; run --without-googletest to disable these tests in make check]))
       AC_CHECK_HEADERS([benchmark/benchmark.h], , AC_MSG_ERROR([benchmark headers for make check not found; run --without-googletest to disable these tests in make check]))
      ], [])
AM_CONDITIONAL([HAVE_GOOGLETEST], [test "x$with_googletest" = "xyes"])

dnl Our Python library relies on cppyy. It can be built without since it is a
dnl pure Python package but we cannot test it if cppyy is not present.
AC_ARG_WITH([pytest], AS_HELP_STRING([--without-pytest], [Do not run Python tests]))
AS_IF([test "x$with_pytest" != "xno" && test "x$have_python" = "xyes"],
      [
       with_pytest=yes
       AS_IF([$PYTHON -c 'import pytest'], , AC_MSG_ERROR([pytest for make check not found; run --without-pytest to disable Python tests in make check]))
       AS_IF([$PYTHON -c 'import cppyy'], , AC_MSG_ERROR([cppyy for make check not found; run --without-pytest to disable Python tests in make check]))
      ], [])
AM_CONDITIONAL([HAVE_PYTEST], [test "x$with_pytest" = "xyes"])

AC_CONFIG_HEADERS([src/libexactreal/exact-real/config.h])
AC_CONFIG_FILES([Makefile src/Makefile src/libexactreal/Makefile src/pyexactreal/Makefile test/Makefile test/libexactreal/Makefile test/pyexactreal/Makefile])

dnl Python's setuptools absolutely want to run in the source directory of the
dnl module. Since this is incompatible with out-of-tree builds of
dnl autoconfiscated packages, we have to copy all the Python bits over to the
dnl build dir manually so that Python can't tell the difference.
AC_CONFIG_LINKS([src/pyexactreal/pyexactreal/__init__.py:src/pyexactreal/pyexactreal/__init__.py
                 src/pyexactreal/pyexactreal/cppyy_exactreal.py:src/pyexactreal/pyexactreal/cppyy_exactreal.py])

AC_OUTPUT
