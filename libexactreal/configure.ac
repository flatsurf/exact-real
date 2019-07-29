dnl Process this file with autoconf to produce a configure script.

dnl We need at least autoconf 2.69 for this configure.ac to work.
AC_PREREQ([2.69])
AC_INIT([libexactreal], [0.0.1], [julian.rueth@fsfe.org])

AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_AUX_DIR([.])
AM_INIT_AUTOMAKE([1.14.1 subdir-objects])

dnl Version number of the library c:r:a.
dnl Set to c+1:0:0 on a non-backwards-compatible change.
dnl Set to c+1:0:a+1 on a backwards-compatible change.
dnl Set to c:r+1:a if the interface is unchanged.
AC_SUBST([libexactreal_version_info], [0:0:0])

AC_CANONICAL_HOST

dnl Find C++ Compiler
AC_PROG_CXX

dnl Set INSTALL or use install-sh.
AC_PROG_INSTALL

dnl We build our library with libtool.
LT_INIT

dnl We run the valgrind test-suite in make check-valgrind (if valgrind is installed)
AX_VALGRIND_DFLT([sgcheck], [off])
AX_VALGRIND_DFLT([drd], [off])
AX_VALGRIND_DFLT([helgrind], [off])
AX_VALGRIND_CHECK()

dnl Check for required libraries
AC_LANG([C++])

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

AC_CONFIG_HEADERS([src/exact-real/config.h])
AC_CONFIG_FILES([Makefile src/Makefile test/Makefile])

AC_OUTPUT
