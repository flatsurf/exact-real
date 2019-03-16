check_PROGRAMS = arb random_real_number rational_real_number module element
AM_DEFAULT_SOURCE_EXT = .test.cc

TESTS = $(check_PROGRAMS)

@VALGRIND_CHECK_RULES@

AM_CPPFLAGS = -I $(srcdir)/../../src/libexactreal -I $(builddir)/../../src/libexactreal
AM_LDFLAGS = $(builddir)/../../src/libexactreal/libexactreal.la -lgtest -lbenchmark -lpthread -lrt -lgmpxx
