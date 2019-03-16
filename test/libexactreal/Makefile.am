check_PROGRAMS = arb random_real_number rational_real_number module element

TESTS = $(check_PROGRAMS)

arb_SOURCES = arb.test.cc main.hpp
random_real_number_SOURCES = random_real_number.test.cc main.hpp
rational_real_number_SOURCES = rational_real_number.test.cc main.hpp
module_SOURCES = module.test.cc main.hpp
element_SOURCES = element.test.cc main.hpp

@VALGRIND_CHECK_RULES@

AM_CPPFLAGS = -I $(srcdir)/../../src/libexactreal -I $(builddir)/../../src/libexactreal
AM_LDFLAGS = $(builddir)/../../src/libexactreal/libexactreal.la -lgtest -lbenchmark -lpthread -lrt -lgmpxx

# Run tests serially so we can see the output without having to check the log file explicitly.
# We might want to change this once the tests take a substantial amount of time.
AUTOMAKE_OPTIONS = serial-tests
