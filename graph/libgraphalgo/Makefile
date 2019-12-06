includes := \
	clique_edwards.h \
	bron.h \
	bron1.h \
	bron2.h \
	greedy_coloring_lf.h \
	christofides.h \
	quick_zykov.h

libgraphalgo_sources := \
	clique_edwards.cc \
	bron.cc \
	bron1.cc \
	bron2.cc \
	greedy_coloring_lf.cc \
	christofides.cc \
	quick_zykov.cc

test_sources := \
	clique_edwards_test.cc \
	greedy_coloring_lf_test.cc \
	bron_test.cc \
	christofides_test.cc \
	quick_zykov_test.cc

sources := $(libgraphalgo_sources) $(test_sources)

libraries := libgraphalgo.a

tests := \
	clique_edwards_test \
	greedy_coloring_lf_test \
	bron_test \
	christofides_test \
	quick_zykov_test

include ../../templates/Makefile.all.mk

CPPFLAGS += -I../libgraphutil -I../libgraphcore -I../../utility/libutil
CXXFLAGS += -I../libgraphutil -I../libgraphcore -I../../utility/libutil

TESTLIBS += ../libgraphutil/libgraphutil.a ../libgraphcore/libgraphcore.a

libgraphalgo.a: $(subst .cc,.o,$(libgraphalgo_sources))
	$(make-library)

clique_edwards_test:	clique_edwards_test.o $(libraries) $(TESTLIBS)
greedy_coloring_lf_test:	greedy_coloring_lf_test.o $(libraries) $(TESTLIBS)
bron_test:	bron_test.o $(libraries) $(TESTLIBS)
christofides_test:	christofides_test.o $(libraries) $(TESTLIBS)
quick_zykov_test:	quick_zykov_test.o $(libraries) $(TESTLIBS)

-include $(subst .cc,.d,$(sources))
