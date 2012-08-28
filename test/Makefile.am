EXTRA_DIST = 
CLEANFILES = test_CBNSimulation.out test_CBNSimulation.wl

check_PROGRAMS = \
	test_CBNSimulation

test_CBNSimulation_SOURCES = test_CBNSimulation.cc
test_CBNSimulation_LDFLAGS = -L$(top_builddir) -lcbnsim

TESTS = test_CBNSimulation

all-local: test_CBNSimulation.wl

test_CBNSimulation.wl: $(top_builddir)/cbnwlgen
	$< -p 20% -s 20% -l 15 -topo $(top_srcdir)/topo/0005.brite -dist-dir $(top_srcdir)/dist -pt 600 f -st 60 f -wtu 0.01 -ttu 0.001 -node-mode c -str-value-dist z -msg-attr-dist z -pred-constr-dist z > $@

INCLUDES = -I$(top_srcdir)
