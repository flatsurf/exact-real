lib_LTLIBRARIES = libexactreal.la

libexactreal_la_SOURCES =               \
    real_number.cc                      \
    real_number_base.cc                 \
    rational_real_number.cc             \
    random_real_number.cc               \
    real_number_product.cc              \
    element.cc                          \
    module.cc                           \
    arb.cc                              \
    integer_ring.cc                     \
    rational_field.cc                   \
    number_field.cc                     \
    constrained_random_real_number.cc   \
    arf.cc                              \
    seed.cc                             \
    legacy/1_4_0/element.cc

libexactreal_la_LDFLAGS = -version-info $(libexactreal_version_info)

if HAVE_VERSION_SCRIPT
libexactreal_la_LDFLAGS += -Wl,--version-script=$(srcdir)/libexactreal.map

EXTRA_libexactreal_la_DEPENDENCIES = $(srcdir)/libexactreal.map
endif

nobase_pkginclude_HEADERS =                                  \
    ../exact-real/arb.hpp                                    \
    ../exact-real/arf.hpp                                    \
    ../exact-real/cereal.hpp                                 \
    ../exact-real/cppyy.hpp                                  \
    ../exact-real/element.hpp                                \
    ../exact-real/exact-real.hpp                             \
    ../exact-real/external/spimpl/spimpl.h                   \
    ../exact-real/forward.hpp                                \
    ../exact-real/integer_ring.hpp                           \
    ../exact-real/module.hpp                                 \
    ../exact-real/number_field.hpp                           \
    ../exact-real/number_field_ideal.hpp                     \
    ../exact-real/rational_field.hpp                         \
    ../exact-real/real_number.hpp                            \
    ../exact-real/seed.hpp                                   \
    ../exact-real/yap/arb.hpp                                \
    ../exact-real/yap/arb_assign_transformation.hpp          \
    ../exact-real/yap/arb_expr.hpp                           \
    ../exact-real/yap/arf.hpp                                \
    ../exact-real/yap/arf_assign_transformation.hpp          \
    ../exact-real/yap/arf_expr.hpp                           \
    ../exact-real/yap/assign_transformation.hpp              \
    ../exact-real/yap/forward.hpp                            \
    ../exact-real/yap/params_transformation.hpp              \
    ../exact-real/yap/prec_expr.hpp                          \
    ../exact-real/yap/prec_transformation.hpp                \
    ../exact-real/yap/round_expr.hpp                         \
    ../exact-real/yap/round_transformation.hpp               \
    ../exact-real/yap/terminal.hpp

noinst_HEADERS =                                               \
    external/gmpxxll/gmpxxll/mpz_class.hpp                     \
    external/hash-combine/hash.hpp                             \
    external/unique-factory/unique-factory/unique-factory.hpp  \
    impl/real_number_base.hpp                                  \
    util/assert.ipp                                            \
    legacy/1_4_0/element.hpp

$(builddir)/../exact-real/local.hpp: $(srcdir)/../exact-real/local.hpp.in Makefile
	mkdir -p $(builddir)/libexactreal
	sed -e 's,[@]libdir[@],$(libdir),g' < $< > $@

# Since we generate local.hpp, we need to search $(builddir) during out-of-tree builds aka VPATH builds
VPATH = $(srcdir) $(builddir)
# Generate local.hpp before performing any compilation
BUILT_SOURCES = ../exact-real/local.hpp
# Distribute local.hpp.in as part of the tarball, but not local.hpp
EXTRA_DIST = ../exact-real/local.hpp.in
# We vendor the header-only library Cereal (serialization with C++) which we
# use in some of our source files.
# Cereal is basically not packaged in any distribution since it is a
# header-only library that people always vendor into their source tree.
EXTRA_DIST += external/cereal/include
AM_CPPFLAGS = -isystem $(srcdir)/external/cereal/include
generatedincludedir = $(includedir)/exact-real
nodist_generatedinclude_HEADERS = ../exact-real/local.hpp

# We are building the libexactreal library so visibility attributes should be
# set, see https://gcc.gnu.org/wiki/Visibility
AM_CPPFLAGS += -DBUILD_LIBEXACTREAL

CLEANFILES = ../exact-real/local.hpp
