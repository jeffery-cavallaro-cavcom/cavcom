includes := graphtypes.h edge.h vertex.h graph.h undirected.h simple.h

libgraph_sources := edge.cc vertex.cc graph.cc

test_sources := edge_test.cc vertex_test.cc simple_test.cc

sources := $(libgraph_sources) $(test_sources)

libraries := libgraph.a

tests := edge_test vertex_test simple_test

include ../../templates/Makefile.all.mk

CXXFLAGS += -I../../utility/libutil
CPPFLAGS += -I../../utility/libutil

libgraph.a: $(subst .cc,.o,$(libgraph_sources))
	$(make-library)

edge_test:	edge_test.o $(libraries) $(TESTLIBS)
vertex_test:	vertex_test.o $(libraries) $(TESTLIBS)
simple_test:	simple_test.o $(libraries) $(TESTLIBS)

-include $(subst .cc,.d,$(sources))
