lib_LTLIBRARIES = libexactreal.la

libexactreal_la_SOURCES =               \
    real_number.cc                      \
    rational_real_number.cc             \
    random_real_number.cc               \
    real_number_product.cc              \
    element.cc                          \
    module.cc                           \
    arb.cc                              \
    integer_ring.cc                     \
    rational_field.cc                   \
    number_field.cc                     \
    constraint_random_real_number.cc    \
    arf.cc                              \
    cppyy.cc

# We vendor https://github.com/fredrik-johansson/arb/pull/286 until it's
# released
libexactreal_la_SOURCES +=              \
    external/arb/arf/load_str.c         \
    external/arb/arb/load_str.c         \
    external/arb/arb/dump_str.c         \
    external/arb/mag/load_str.c         \
    external/arb/mag/dump_str.c

libexactreal_la_LDFLAGS = -version-info $(libexactreal_version_info)

nobase_include_HEADERS =                                  \
    exact-real/real_number.hpp                            \
    exact-real/element.hpp                                \
    exact-real/module.hpp                                 \
    exact-real/integer_ring.hpp                           \
    exact-real/rational_field.hpp                         \
    exact-real/number_field.hpp                           \
    exact-real/number_field_ideal.hpp                     \
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
    exact-real/cereal.hpp                                 \
    exact-real/cppyy.hpp

noinst_HEADERS =                                          \
    external/unique-factory/unique_factory.hpp            \
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
# Distribute the bits of arb that we vendor until they are release
EXTRA_DIST += external/arb/arb.h external/arb/arf.h external/arb/mag.h external/arb/arb/load_str.c external/arb/arb/dump_str.c external/arb/arf/load_str.c external/arb/mag/load_str.c external/arb/mag/dump_str.c
# We vendor the header-only library Cereal (serialization with C++) which we
# use in some of our source files.
# Cereal is basically not packaged in any distribution since it is a
# header-only library that people always vendor into their source tree.
EXTRA_DIST += external/cereal/include
AM_CPPFLAGS = -isystem $(srcdir)/external/cereal/include
# Built against the experimental arb headers so that the dump/load functions are known
AM_CPPFLAGS += -isystem $(srcdir)/external/arb/
generatedincludedir = $(includedir)/exact-real
nodist_generatedinclude_HEADERS = exact-real/exact-real.hpp

CLEANFILES = exact-real/exact-real.hpp
