includes := vertex.h vertices.h

libutil_sources := vertex.cc vertices.cc

test_sources := vertex_test.cc vertices_test.cc

sources := $(libutil_sources) $(test_sources)

libraries := libgraph.a

tests := vertex_test vertices_test

include ../../templates/Makefile.all.mk

CXXFLAGS += -I../../utility/libutil
CPPFLAGS += -I../../utility/libutil

libgraph.a: $(subst .cc,.o,$(libutil_sources))
	$(make-library)

vertex_test:	vertex_test.o $(libraries) $(TESTLIBS)
vertices_test:	vertices_test.o $(libraries) $(TESTLIBS)

-include $(subst .cc,.d,$(sources))
