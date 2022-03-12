TESTS =                                                      \
	$(abs_srcdir)/../../exact-real/element.hpp                 \
	$(abs_srcdir)/../../exact-real/arb.hpp                     \
	$(abs_srcdir)/../../exact-real/arf.hpp                     \
	$(abs_srcdir)/../../exact-real/cereal.hpp                  \
	$(abs_srcdir)/../../exact-real/cppyy.hpp                   \
	$(abs_srcdir)/../../exact-real/element.hpp                 \
	$(abs_srcdir)/../../exact-real/exact-real.hpp              \
	$(abs_srcdir)/../../exact-real/forward.hpp                 \
	$(abs_srcdir)/../../exact-real/integer_ring.hpp            \
	$(abs_srcdir)/../../exact-real/module.hpp                  \
	$(abs_srcdir)/../../exact-real/number_field.hpp            \
	$(abs_srcdir)/../../exact-real/rational_field.hpp          \
	$(abs_srcdir)/../../exact-real/real_number.hpp             \
	$(abs_srcdir)/../../exact-real/seed.hpp                    \
	$(abs_srcdir)/../../../doc/manual/libexactreal/index.md    \
	$(abs_srcdir)/../../../doc/manual/libexactreal/overview.md


TEST_EXTENSIONS = .hpp .md
HPP_LOG_COMPILER = $(builddir)/manual-doctest.sh
MD_LOG_COMPILER = $(builddir)/manual-doctest.sh

EXTRA_DIST = $(TESTS)

AM_TESTS_ENVIRONMENT = . $(builddir)/test-env.sh;

@VALGRIND_CHECK_RULES@

BUILT_SOURCES = test-env.sh bin/python manual-doctest.sh
EXTRA_DIST += test-env.sh.in bin/python.in manual-doctest.sh.in
CLEANFILES = test-env.sh bin/python manual-doctest.sh

$(builddir)/test-env.sh: $(srcdir)/test-env.sh.in Makefile
	sed -e 's,[@]abs_builddir[@],$(abs_builddir),g' < $< > $@

$(builddir)/bin/python: $(srcdir)/bin/python.in Makefile
	mkdir -p $(builddir)/bin
	sed -e 's,[@]abs_srcdir[@],$(abs_srcdir),g' -e 's,[@]abs_builddir[@],$(abs_builddir),g' < $< > $@
	chmod +x $@

$(builddir)/manual-doctest.sh: $(srcdir)/manual-doctest.sh.in Makefile
	sed -e 's,[@]abs_srcdir[@],$(abs_srcdir),g' < $< > $@
	chmod +x $@
