noinst_PROGRAMS = benchmark

benchmark_SOURCES = main.cc arb.benchmark.cc random_real_number.benchmark.cc element.benchmark.cc

AM_CPPFLAGS = -I $(srcdir)/.. -I $(builddir)/..
LDADD = $(builddir)/../src/libexactreal.la
LDADD += -lgmp
# Google Benchmark and its dependencies
if OS_LINUX
LDADD += -lbenchmark -lrt -lpthread
else
LDADD += -lbenchmark -lpthread
endif
