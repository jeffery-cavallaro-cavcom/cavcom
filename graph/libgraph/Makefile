includes := graphtypes.h edge.h vertex.h vertices.h errors.h connections.h

libgraph_sources := edge.cc vertex.cc vertices.cc errors.cc connections.cc

test_sources := edge_test.cc vertex_test.cc vertices_test.cc errors_test.cc connections_test.cc

sources := $(libgraph_sources) $(test_sources)

libraries := libgraph.a

tests := edge_test vertex_test vertices_test errors_test connections_test

include ../../templates/Makefile.all.mk

CXXFLAGS += -I../../utility/libutil
CPPFLAGS += -I../../utility/libutil

libgraph.a: $(subst .cc,.o,$(libgraph_sources))
	$(make-library)

edge_test:	edge_test.o $(libraries) $(TESTLIBS)
vertex_test:	vertex_test.o $(libraries) $(TESTLIBS)
vertices_test:	vertices_test.o $(libraries) $(TESTLIBS)
errors_test:	errors_test.o $(libraries) $(TESTLIBS)
connections_test:	connections_test.o $(libraries) $(TESTLIBS)

-include $(subst .cc,.d,$(sources))
