dnl Process this file with autoconf to produce a configure script.

dnl We need at least autoconf 2.69 for this configure.ac to work.
AC_PREREQ([2.69])
AC_INIT([exact-real-doc], [2.1.0], [julian.rueth@fsfe.org])

AC_CONFIG_AUX_DIR([.])
AM_INIT_AUTOMAKE([1.14.1 subdir-objects tar-pax foreign])

AM_SILENT_RULES([yes]) # non-verbose make; override with ./configure --disable-silent-rules

AC_CANONICAL_HOST

dnl Set INSTALL or use install-sh.
AC_PROG_INSTALL

AC_PATH_PROG([SPHINXBUILD], [sphinx-build], [no])
AS_IF([test x"$ac_cv_path_SPHINXBUILD" == x"no"], [AC_MSG_ERROR([sphinx-build could not be found. It is required to build the documentation. Use --without-doc to not build the documentation of exact-real.])])
AC_SUBST([SPHINXBUILD], [$ac_cv_path_SPHINXBUILD])

AC_PATH_PROG([STANDARDESE], [standardese], [no])
AS_IF([test x"$ac_cv_path_STANDARDESE" == x"no"], [AC_MSG_ERROR([standardese could not be found. It is required to build the documentation. Use --without-doc to not build the documentation of exact-real.])])
AC_SUBST([STANDARDESE], [$ac_cv_path_STANDARDESE])

AC_PATH_PROG([MKDOCS], [mkdocs], [no])
AS_IF([test x"$ac_cv_path_MKDOCS" == x"no"], [AC_MSG_ERROR([MkDocs could not be found. It is required to build the documentation. Use --without-doc to not build the documentation of exact-real.])])
AC_SUBST([MKDOCS], [$ac_cv_path_MKDOCS])

AC_CONFIG_FILES([Makefile manual/Makefile])

AC_OUTPUT

