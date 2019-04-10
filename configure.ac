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

dnl We run the valgrind test-suite in make check-valgrind
AX_VALGRIND_DFLT([sgcheck], [off])
AX_VALGRIND_DFLT([drd], [off])
AX_VALGRIND_DFLT([helgrind], [off])
AX_VALGRIND_CHECK()

dnl Check for required libraries
AC_LANG([C++])

dnl We use some C++17 features such as std::optional
AX_CXX_COMPILE_STDCXX(17)

ACX_PTHREAD([], [AC_MSG_ERROR([pthread not supported])])

AC_CHECK_HEADERS([arb.h], , AC_MSG_ERROR([Arb headers not found]))
AC_CHECK_LIB([arb], [arb_init], , AC_MSG_ERROR([Arb library not found]), [-lgmp -lmpfr -lflint])

AC_CHECK_HEADERS([gmpxx.h], , AC_MSG_ERROR([GMP headers for C++ not found]))
# GMPXX does not contain anything that we can check for with AX_CXX_CHECK_LIB
# so we just check for something from the standard library, i.e., that -lgmpxx
# goes through
AX_CXX_CHECK_LIB([gmpxx], [std::runtime_error::what () const], , AC_MSG_ERROR([GMP library for C++ not found]), [-lgmp -lmpfr -lflint])

AC_CHECK_HEADERS([e-antic/renfxx.h], , AC_MSG_ERROR([E-ANTIC headers not found]))
AC_CHECK_LIB([eantic], [renf_elem_clear], , AC_MSG_ERROR([E-ANTIC library not found]), [-lgmp -lmpfr -lflint])
AX_CXX_CHECK_LIB([eanticxx], [eantic::renf_elem_class], , AC_MSG_ERROR([E-ANTIC library not found]), [-lgmp -lmpfr -lflint])

AC_CHECK_HEADERS([gtest/gtest.h], , AC_MSG_ERROR([googletest headers for make check not found]))
AC_CHECK_HEADERS([benchmark/benchmark.h], , AC_MSG_ERROR([googletest headers for make check not found]))

AC_CONFIG_HEADERS([src/libexactreal/exact-real/config.h])
AC_CONFIG_FILES([Makefile src/Makefile src/libexactreal/Makefile test/Makefile test/libexactreal/Makefile])
AC_OUTPUT
