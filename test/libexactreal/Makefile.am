check_PROGRAMS = arb arb_yap arb_benchmark random_real_number rational_real_number module element constraint_random_real_number arf_yap arf

TESTS = $(check_PROGRAMS)

arb_SOURCES = arb.test.cc arb.hpp main.hpp
arf_SOURCES = arf.test.cc arf.hpp main.hpp
arb_yap_SOURCES = arb.yap.test.cc arb.hpp main.hpp
arf_yap_SOURCES = arf.yap.test.cc arf.hpp main.hpp
arb_benchmark_SOURCES = arb.benchmark.cc arb.hpp main.hpp
random_real_number_SOURCES = random_real_number.test.cc main.hpp arf.test.hpp
rational_real_number_SOURCES = rational_real_number.test.cc main.hpp
module_SOURCES = module.test.cc main.hpp
element_SOURCES = element.test.cc main.hpp
constraint_random_real_number_SOURCES = constraint_random_real_number.test.cc main.hpp arf.test.hpp

@VALGRIND_CHECK_RULES@

AM_CPPFLAGS = -I $(srcdir)/../../src/libexactreal -I $(builddir)/../../src/libexactreal
AM_LDFLAGS = $(builddir)/../../src/libexactreal/libexactreal.la -lgtest -lbenchmark -lrt

# Run tests serially so we can see the output without having to check the log file explicitly.
# AUTOMAKE_OPTIONS = serial-tests
