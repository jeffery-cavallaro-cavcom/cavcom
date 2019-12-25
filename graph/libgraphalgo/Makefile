includes := \
	mycielski.h \
	hopcroft.h \
	clique_edwards.h \
	bron.h \
	bron1.h \
	bron2.h \
	greedy_coloring.h \
	christofides.h \
	chromatic_wang.h \
	zykov.h \
	quick_zykov.h

libgraphalgo_sources := \
	mycielski.cc \
	hopcroft.cc \
	clique_edwards.cc \
	bron.cc \
	bron1.cc \
	bron2.cc \
	greedy_coloring.cc \
	christofides.cc \
	chromatic_wang.cc \
	zykov.cc \
	quick_zykov.cc

test_sources := \
	mycielski_test.cc \
	hopcroft_test.cc \
	clique_edwards_test.cc \
	greedy_coloring_test.cc \
	bron_test.cc \
	christofides_test.cc \
	chromatic_wang_test.cc \
	zykov_test.cc \
	quick_zykov_test.cc

sources := qz_example.cc $(libgraphalgo_sources) $(test_sources)

programs := qz_example

libraries := libgraphalgo.a

tests := \
	mycielski_test \
	hopcroft_test \
	clique_edwards_test \
	greedy_coloring_test \
	bron_test \
	christofides_test \
	chromatic_wang_test \
	zykov_test \
	quick_zykov_test

include ../../templates/Makefile.all.mk

CPPFLAGS += -I../libgraphutil -I../libgraphcore -I../../utility/libutil
CXXFLAGS += -I../libgraphutil -I../libgraphcore -I../../utility/libutil

TESTLIBS += ../libgraphutil/libgraphutil.a ../libgraphcore/libgraphcore.a ../../utility/libutil/libutil.a

libgraphalgo.a: $(subst .cc,.o,$(libgraphalgo_sources))
	$(make-library)

qz_example:	qz_example.o $(libraries) $(TESTLIBS)

mycielski_test:	mycielski_test.o $(libraries) $(TESTLIBS)
hopcroft_test:	hopcroft_test.o $(libraries) $(TESTLIBS)
clique_edwards_test:	clique_edwards_test.o $(libraries) $(TESTLIBS)
greedy_coloring_test:	greedy_coloring_test.o $(libraries) $(TESTLIBS)
bron_test:	bron_test.o $(libraries) $(TESTLIBS)
christofides_test:	christofides_test.o $(libraries) $(TESTLIBS)
chromatic_wang_test:	chromatic_wang_test.o $(libraries) $(TESTLIBS)
zykov_test:	zykov_test.o $(libraries) $(TESTLIBS)
quick_zykov_test:	quick_zykov_test.o $(libraries) $(TESTLIBS)

-include $(subst .cc,.d,$(sources))
