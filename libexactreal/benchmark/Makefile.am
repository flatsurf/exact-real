noinst_PROGRAMS = benchmark

benchmark_SOURCES = main.cc arb.benchmark.cc random_real_number.benchmark.cc element.benchmark.cc

AM_CPPFLAGS = -I $(srcdir)/.. -I $(builddir)/..
AM_LDFLAGS = $(builddir)/../src/libexactreal.la
AM_LDFLAGS += -lgmp
# Google Benchmark and its dependencies
AM_LDFLAGS += -lbenchmark -lrt -lpthread

