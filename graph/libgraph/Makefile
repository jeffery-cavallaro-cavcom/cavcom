includes := graph.h undirected.h simple.h

libgraph_sources := graph.cc

test_sources := simple_test.cc

sources := $(libgraph_sources) $(test_sources)

libraries := libgraph.a

tests := simple_test

include ../../templates/Makefile.all.mk

CXXFLAGS += -I../../utility/libutil
CPPFLAGS += -I../../utility/libutil

libgraph.a: $(subst .cc,.o,$(libgraph_sources))
	$(make-library)

simple_test:	simple_test.o $(libraries) $(TESTLIBS)

-include $(subst .cc,.d,$(sources))
