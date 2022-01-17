dnl Process this file with autoconf to produce a configure script.

dnl We need at least autoconf 2.69 for this configure.ac to work.
AC_PREREQ([2.69])
AC_INIT([libexactreal], [2.3.0], [julian.rueth@fsfe.org])

AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_AUX_DIR([.])
AM_INIT_AUTOMAKE([1.14.1 tar-pax foreign subdir-objects])

dnl Version number of the library c:r:a.
dnl Set to c+1:0:0 on a non-backwards-compatible change.
dnl Set to c+1:0:a+1 on a backwards-compatible change.
dnl Set to c:r+1:a if the interface is unchanged.
AC_SUBST([libexactreal_version_info], [8:0:2])

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

AC_CHECK_HEADERS([arb.h], , AC_MSG_ERROR([Arb headers not found]))
AC_CHECK_LIB([arb], [arb_load_str], , AC_MSG_ERROR([Arb library >=2.17.0 not found]), [-lgmp -lmpfr -lflint])

AC_CHECK_HEADERS([gmpxx.h], , AC_MSG_ERROR([GMP headers for C++ not found]))
dnl GMPXX does not contain anything that we can check for with AX_CXX_CHECK_LIB
dnl so we just check for something from the standard library, i.e., that -lgmpxx
dnl goes through
AX_CXX_CHECK_LIB([gmpxx], [std::runtime_error::what () const], , AC_MSG_ERROR([GMP library for C++ not found]), [-lgmp -lmpfr -lflint])

AC_CHECK_HEADERS([e-antic/renfxx.h], , AC_MSG_ERROR([E-ANTIC headers not found]))
AC_CHECK_LIB([eantic], [renf_elem_clear], , AC_MSG_ERROR([E-ANTIC library not found]), [-lgmp -lmpfr -lflint])
AX_CXX_CHECK_LIB([eanticxx], [eantic::renf_elem_class], , AC_MSG_ERROR([E-ANTIC library not found]), [-lgmp -lmpfr -lflint])
AC_COMPILE_IFELSE(
  [AC_LANG_PROGRAM(
   [[#include <e-antic/renfxx.h>]],
   [[const eantic::renf_class& K = eantic::renf_elem_class().parent();]])], , AC_MSG_ERROR([[E-ANTIC library incompatible, probably too old]]))

dnl Our benchmarks use Google's C++ benchmark library.
dnl We fail if they cannot be found but let the user disable it explicitly.
AC_ARG_WITH([benchmark], AS_HELP_STRING([--without-benchmark], [Do not build C++ benchmarks that require google/benchmark]))
AS_IF([test "x$with_benchmark" != "xno"],
      [
       with_benchmark=yes
       AC_CHECK_HEADERS([benchmark/benchmark.h], , AC_MSG_ERROR([benchmark headers not found; run --without-benchmark to disable building of benchmark/]))
      ], [])
AM_CONDITIONAL([HAVE_BENCHMARK], [test "x$with_benchmark" = "xyes"])

AC_ARG_WITH([version-script], AS_HELP_STRING([--with-version-script], [Only export a curated list of symbols in the shared library.]))
AM_CONDITIONAL([HAVE_VERSION_SCRIPT], [test "x$with_version_script" = "xyes"])

AC_CONFIG_HEADERS([exact-real/config.h])
AC_CONFIG_FILES([Makefile src/Makefile test/Makefile benchmark/Makefile])

AC_OUTPUT
