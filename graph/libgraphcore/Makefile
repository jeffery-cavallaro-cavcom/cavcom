includes :=	graphtypes.h \
		edge.h vertex.h \
		vertices.h \
		errors.h \
		connections.h \
		graph.h \
		simple_graph.h

libgraphcore_sources :=	edge.cc \
			vertex.cc \
			vertices.cc \
			errors.cc \
			connections.cc \
			graph.cc \
			simple_graph.cc

test_sources :=	edge_test.cc \
		vertex_test.cc \
		vertices_test.cc \
		errors_test.cc \
		connections_test.cc \
		simple_graph_test.cc

sources := $(libgraphcore_sources) $(test_sources)

libraries := libgraphcore.a

tests := edge_test vertex_test vertices_test errors_test connections_test simple_graph_test

include ../../templates/Makefile.all.mk

CXXFLAGS += -I../../utility/libutil
CPPFLAGS += -I../../utility/libutil

libgraphcore.a: $(subst .cc,.o,$(libgraphcore_sources))
	$(make-library)

edge_test:	edge_test.o $(libraries) $(TESTLIBS)
vertex_test:	vertex_test.o $(libraries) $(TESTLIBS)
vertices_test:	vertices_test.o $(libraries) $(TESTLIBS)
errors_test:	errors_test.o $(libraries) $(TESTLIBS)
connections_test:	connections_test.o $(libraries) $(TESTLIBS)
simple_graph_test:	simple_graph_test.o $(libraries) $(TESTLIBS)

-include $(subst .cc,.d,$(sources))
