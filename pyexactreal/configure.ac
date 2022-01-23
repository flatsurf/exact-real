dnl Process this file with autoconf to produce a configure script.

dnl We need at least autoconf 2.69 for this configure.ac to work.
AC_PREREQ([2.69])
AC_INIT([pyexactreal], [2.3.0], [julian.rueth@fsfe.org])
AC_CONFIG_AUX_DIR([.])

AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE([1.14.1 tar-pax foreign])

dnl We run the valgrind test-suite in make check-valgrind (if valgrind is installed)
AX_VALGRIND_DFLT([sgcheck], [off])
AX_VALGRIND_DFLT([drd], [off])
AX_VALGRIND_DFLT([helgrind], [off])
AX_VALGRIND_CHECK()

dnl We provide a Python module powered by cppyy
AC_ARG_WITH([python], AS_HELP_STRING([--without-python], [Do not build Python module]))
AS_IF([test "x$with_python" != "xno"],
      [AM_PATH_PYTHON([3.6], [have_python=yes], [have_python=no])],
      [have_python=no])
AS_IF([test "x$have_python" = "xyes"],
      [],
      [AS_IF([test "x$with_python" = "xyes"], [AC_MSG_ERROR([Python package requested but Python not found])])])
AM_CONDITIONAL([HAVE_PYTHON], [test "x$have_python" = "xyes"])

dnl Our Python library relies on cppyy. It can be built without since it is a
dnl pure Python package but we cannot test it if cppyy is not present.
AC_ARG_WITH([pytest], AS_HELP_STRING([--without-pytest], [Do not run Python tests]))
AS_IF([test "x$with_pytest" != "xno" && test "x$have_python" = "xyes"],
      [
       with_pytest=yes
       AS_IF([$PYTHON -c 'import pytest'], , AC_MSG_ERROR([pytest for make check not found; run --without-pytest to disable Python tests in make check]))
       AS_IF([$PYTHON -c 'import cppyy'], , AC_MSG_ERROR([cppyy for make check not found; run --without-pytest to disable Python tests in make check]))
       AS_IF([$PYTHON -c 'import pyeantic'], , AC_MSG_ERROR([pyeantic for make check not found; run --without-pytest to disable Python tests in make check]))
       AS_IF([$PYTHON -c 'import gmpxxyy'], , AC_MSG_ERROR([gmpxxyy for make check not found; run --without-pytest to disable Python tests in make check]))
      ], [])
AM_CONDITIONAL([HAVE_PYTEST], [test "x$with_pytest" = "xyes"])

dnl Some bits require SageMath to be installed. We can build without it since it is a pure Python package but we cannot test it then.
AC_ARG_WITH([sage], AS_HELP_STRING([--without-sage], [Do not run SageMath tests]))
AS_IF([test "x$with_sage" != "xno" && test "x$with_pytest" = "xyes"],
      [
       with_sage=yes
       AC_MSG_CHECKING([for sage module])
       AS_IF([$PYTHON -c 'import sage'], , AC_MSG_ERROR([SageMath for make check not found; run --without-sage to disable SageMath tests in make check]))
       AC_MSG_RESULT([yes])
      ], [])
AM_CONDITIONAL([HAVE_SAGE], [test "x$with_sage" = "xyes"])

AC_CONFIG_FILES([Makefile src/Makefile test/Makefile])

AC_OUTPUT

