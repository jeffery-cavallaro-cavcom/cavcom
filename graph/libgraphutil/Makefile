includes := \
	formatter.h \
	tikz_formatter.h \
	random_graph.h \
	graph_algorithm.h \
	component_number_algorithm.h \
	component_algorithm.h \
	clique_number_algorithm.h \
	clique_algorithm.h \
	chromatic_number_algorithm.h \
	vertex_coloring_algorithm.h

libgraphutil_sources := \
	tikz_formatter.cc \
	random_graph.cc \
	graph_algorithm.cc \
	component_number_algorithm.cc \
	component_algorithm.cc \
	clique_number_algorithm.cc \
	clique_algorithm.cc \
	chromatic_number_algorithm.cc \
	vertex_coloring_algorithm.cc

test_sources := \
	formatter_test.cc \
	tikz_formatter_test.cc \
	random_graph_test.cc \
	graph_algorithm_test.cc \
	component_algorithm_test.cc \
	clique_algorithm_test.cc \
	vertex_coloring_algorithm_test.cc

sources := $(libgraphutil_sources) $(test_sources)

libraries := libgraphutil.a

tests := \
	formatter_test \
	tikz_formatter_test \
	random_graph_test \
	graph_algorithm_test \
	component_algorithm_test \
	clique_algorithm_test \
	vertex_coloring_algorithm_test

include ../../templates/Makefile.all.mk

CPPFLAGS += -I../libgraphcore -I../../utility/libutil
CXXFLAGS += -I../libgraphcore -I../../utility/libutil

TESTLIBS += ../libgraphcore/libgraphcore.a

libgraphutil.a: $(subst .cc,.o,$(libgraphutil_sources))
	$(make-library)

formatter_test:	formatter_test.o $(libraries) $(TESTLIBS)
tikz_formatter_test:	tikz_formatter_test.o $(libraries) $(TESTLIBS)
random_graph_test:	random_graph_test.o $(libraries) $(TESTLIBS)
graph_algorithm_test:	graph_algorithm_test.o $(libraries) $(TESTLIBS)
component_algorithm_test:	component_algorithm_test.o $(libraries) $(TESTLIBS)
clique_algorithm_test:	clique_algorithm_test.o $(libraries) $(TESTLIBS)
vertex_coloring_algorithm_test:	vertex_coloring_algorithm_test.o $(libraries) $(TESTLIBS)

-include $(subst .cc,.d,$(sources))
