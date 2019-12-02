includes := formatter.h tikz_formatter.h random_graph.h graph_algorithm.h clique_algorithm.h

libgraphutil_sources := tikz_formatter.cc random_graph.cc graph_algorithm.cc

test_sources := formatter_test.cc tikz_formatter_test.cc random_graph_test.cc graph_algorithm_test.cc

sources := $(libgraphutil_sources) $(test_sources)

libraries := libgraphutil.a

tests := formatter_test tikz_formatter_test random_graph_test graph_algorithm_test

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

-include $(subst .cc,.d,$(sources))
