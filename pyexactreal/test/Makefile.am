if HAVE_SAGE
  MAYBE_SAGE = sage-doctest.sh
endif
TESTS = arf.py arb.py real_number.py module.py binder.py element.py python-doctest.sh ${MAYBE_SAGE}
EXTRA_DIST = $(TESTS)

AM_TESTS_ENVIRONMENT = . $(builddir)/test-env.sh;

arf.py: test-env.sh
arb.py: test-env.sh
real_number.py: test-env.sh
module.py: test-env.sh
binder.py: test-env.sh
element.py: test-env.sh

@VALGRIND_CHECK_RULES@

BUILT_SOURCES = test-env.sh python-doctest.sh sage-doctest.sh
EXTRA_DIST += test-env.sh.in python-doctest.sh.in sage-doctest.sh.in
CLEANFILES = test-env.sh python-doctest.sh sage-doctest.sh
$(builddir)/test-env.sh: $(srcdir)/test-env.sh.in Makefile
	sed -e 's,[@]abs_srcdir[@],$(abs_srcdir),g' -e 's,[@]abs_builddir[@],$(abs_builddir),g' -e 's,[@]pythondir[@],$(pythondir),g' < $< > $@
$(builddir)/python-doctest.sh: $(srcdir)/python-doctest.sh.in Makefile
	sed -e 's,[@]srcdir[@],$(srcdir),g' -e 's,[@]builddir[@],$(builddir),g' < $< > $@
	chmod +x $@
$(builddir)/sage-doctest.sh: $(srcdir)/sage-doctest.sh.in Makefile
	sed -e 's,[@]srcdir[@],$(srcdir),g' -e 's,[@]builddir[@],$(builddir),g' < $< > $@
	chmod +x $@
