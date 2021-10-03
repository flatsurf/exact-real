dnl Process this file with autoconf to produce a configure script.

dnl We need at least autoconf 2.69 for this configure.ac to work.
AC_PREREQ([2.69])
AC_INIT([exact-real], [2.1.0], [julian.rueth@fsfe.org])

AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE([1.14.1 tar-pax foreign])

AC_CONFIG_FILES([Makefile])

AC_CONFIG_SUBDIRS([libexactreal])

AC_ARG_WITH([pyexactreal], AS_HELP_STRING([--without-pyexactreal], [Do not build the Python interface pyexactreal]))
AS_IF([test "x$with_pyexactreal" != "xno"], [AC_CONFIG_SUBDIRS([pyexactreal])])

AC_ARG_WITH([doc], AS_HELP_STRING([--without-doc], [Do not build the exact-real documentation]))
AS_IF([test "x$with_doc" != "xno"], [AC_CONFIG_SUBDIRS([doc])])

AC_OUTPUT
