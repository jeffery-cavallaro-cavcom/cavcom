includes := graphtypes.h edge.h vertex.h vertices.h

libgraph_sources := edge.cc vertex.cc vertices.cc

test_sources := edge_test.cc vertex_test.cc vertices_test.cc

sources := $(libgraph_sources) $(test_sources)

libraries := libgraph.a

tests := edge_test vertex_test vertices_test

include ../../templates/Makefile.all.mk

CXXFLAGS += -I../../utility/libutil
CPPFLAGS += -I../../utility/libutil

libgraph.a: $(subst .cc,.o,$(libgraph_sources))
	$(make-library)

edge_test:	edge_test.o $(libraries) $(TESTLIBS)
vertex_test:	vertex_test.o $(libraries) $(TESTLIBS)
vertices_test:	vertices_test.o $(libraries) $(TESTLIBS)

-include $(subst .cc,.d,$(sources))
