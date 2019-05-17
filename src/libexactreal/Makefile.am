lib_LTLIBRARIES = libexactreal.la

libexactreal_la_SOURCES =               \
    real_number.cc                      \
    rational_real_number.cc             \
    random_real_number.cc               \
		real_number_product.cc              \
    element.cc                          \
    module.cc                           \
    arb.cc                              \
    integer_ring_traits.cc              \
    rational_field_traits.cc            \
    number_field_traits.cc              \
    constraint_random_real_number.cc    \
    arf.cc                              \
    cppyy.cc

libexactreal_la_LDFLAGS = -version-info $(libexactreal_version_info)

nobase_include_HEADERS =                                  \
    exact-real/real_number.hpp                            \
    exact-real/element.hpp                                \
    exact-real/module.hpp                                 \
    exact-real/integer_ring_traits.hpp                    \
    exact-real/rational_field_traits.hpp                  \
    exact-real/number_field_traits.hpp                    \
    exact-real/number_field_ideal_traits.hpp              \
    exact-real/ring_traits.hpp                            \
    exact-real/arf.hpp                                    \
    exact-real/arb.hpp                                    \
    exact-real/forward.hpp                                \
    exact-real/yap/arb_assign_transformation.hpp          \
    exact-real/yap/arb_expr.hpp                           \
    exact-real/yap/arb.hpp                                \
    exact-real/yap/arf_assign_transformation.hpp          \
    exact-real/yap/arf_expr.hpp                           \
    exact-real/yap/arf.hpp                                \
    exact-real/yap/assign_transformation.hpp              \
    exact-real/yap/forward.hpp                            \
    exact-real/yap/params_transformation.hpp              \
    exact-real/yap/prec_expr.hpp                          \
    exact-real/yap/prec_transformation.hpp                \
    exact-real/yap/round_expr.hpp                         \
    exact-real/yap/round_transformation.hpp               \
    exact-real/yap/terminal.hpp                           \
    exact-real/external/spimpl/spimpl.h                   \
    exact-real/cppyy.hpp

noinst_HEADERS =                                          \
		exact-real/detail/unique_factory.hpp                  \
		exact-real/detail/smart_ostream.hpp

$(builddir)/exact-real/exact-real.hpp: $(srcdir)/exact-real/exact-real.hpp.in Makefile
	mkdir -p $(builddir)/libexactreal
	sed -e 's,[@]libdir[@],$(libdir),g' < $< > $@

# Since we generate exact-real.hpp, we need to search $(builddir) during out-of-tree builds aka VPATH builds
VPATH = $(srcdir) $(builddir)
# Generate exact-real.hpp before performing any compilation
BUILT_SOURCES = exact-real/exact-real.hpp
# Distribute exact-real.hpp.in as part of the tarball, but not exact-real.hpp
EXTRA_DIST = exact-real/exact-real.hpp.in
nodist_pkginclude_HEADERS = exact-real/exact-real.hpp

CLEANFILES = exact-real/exact-real.hpp
