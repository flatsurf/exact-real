check_PROGRAMS = arb arb_yap real_number_product random_real_number rational_real_number module element constraint_random_real_number arf_yap arf cereal cppyy legacy_1_4_0_element

TESTS = $(check_PROGRAMS)

arb_SOURCES = arb.test.cc arb.hpp main.cc
arf_SOURCES = arf.test.cc arf.hpp main.cc
arb_yap_SOURCES = arb.yap.test.cc arb.hpp main.cc
arf_yap_SOURCES = arf.yap.test.cc arf.hpp main.cc
cereal_SOURCES = cereal.test.cc arb.hpp arf.hpp main.cc
random_real_number_SOURCES = random_real_number.test.cc main.cc arf.test.hpp
rational_real_number_SOURCES = rational_real_number.test.cc main.cc
module_SOURCES = module.test.cc main.cc
element_SOURCES = element.test.cc element_generator.hpp module_generator.hpp main.cc
constraint_random_real_number_SOURCES = constraint_random_real_number.test.cc main.cc arf.test.hpp
cppyy_SOURCES = cppyy.test.cc main.cc
real_number_product_SOURCES = real_number_product.test.cc main.cc arf.test.hpp
legacy_1_4_0_element_SOURCES = legacy/1_4_0/element.test.cc main.cc

# We vendor the header-only library Cereal (serialization with C++ to be able
# to run the tests even when cereal is not installed.
# Cereal is basically not packaged in any distribution since it is a
# header-only library that people always vendor into their source tree.
EXTRA_DIST = external/cereal/include
# We vendor the header-only test library catch2
EXTRA_DIST += external/catch2/single_include/catch2/catch.hpp

@VALGRIND_CHECK_RULES@

AM_CPPFLAGS = -I $(srcdir)/../src/ -I $(builddir)/../src/ -isystem $(srcdir)/external/cereal/include
AM_LDFLAGS = $(builddir)/../src/libexactreal.la
# arb.hpp & arf.hpp use gmpxx; it appears that we need to link gmp in explicitly as well to make gmpxx happy
AM_LDFLAGS += -lgmpxx -lgmp
# arb.hpp & arf.hpp use flint
AM_LDFLAGS += -lflint
