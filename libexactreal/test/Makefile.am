if HAVE_GOOGLETEST
  check_PROGRAMS = arb arb_yap arb_benchmark random_real_number rational_real_number module element constraint_random_real_number arf_yap arf cereal
  TESTS = $(check_PROGRAMS)
endif

arb_SOURCES = arb.test.cc arb.hpp main.hpp
arf_SOURCES = arf.test.cc arf.hpp main.hpp
arb_yap_SOURCES = arb.yap.test.cc arb.hpp main.hpp
arf_yap_SOURCES = arf.yap.test.cc arf.hpp main.hpp
arb_benchmark_SOURCES = arb.benchmark.cc arb.hpp main.hpp
cereal_SOURCES = cereal.test.cc arb.hpp arf.hpp main.hpp rings.hpp
random_real_number_SOURCES = random_real_number.test.cc main.hpp arf.test.hpp
rational_real_number_SOURCES = rational_real_number.test.cc main.hpp
module_SOURCES = module.test.cc main.hpp
element_SOURCES = element.test.cc main.hpp
constraint_random_real_number_SOURCES = constraint_random_real_number.test.cc main.hpp arf.test.hpp

# We vendor the header-only library Cereal (serialization of the C++
# structures) to be able to run the tests even when cereal is not installed.
# Cereal is basically not packaged in any distribution since it is a
# header-only library that people always vendor into their sourc tree.
EXTRA_DIST = external/cereal/include

@VALGRIND_CHECK_RULES@

AM_CPPFLAGS = -I $(srcdir)/../src/ -I $(builddir)/../src/ -isystem $(srcdir)/external/cereal/include
AM_LDFLAGS = $(builddir)/../src/libexactreal.la -lgtest -lbenchmark -lrt -lpthread
# arb.hpp & arf.hpp use gmpxx; it appears that we need to link gmp in explicitly as well to make gmpxx happy
AM_LDFLAGS += -lgmpxx -lgmp
# arb.hpp & arf.hpp use flint
AM_LDFLAGS += -lflint
