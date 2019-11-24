includes := quick_zykov.h bron.h bron1.h bron2.h

libgraphalgo_sources := quick_zykov.cc bron.cc bron1.cc bron2.cc

test_sources := quick_zykov_test.cc bron1_test.cc bron2_test.cc

sources := $(libgraphalgo_sources) $(test_sources)

libraries := libgraphalgo.a

tests := quick_zykov_test bron1_test bron2_test

include ../../templates/Makefile.all.mk

CPPFLAGS += -I../libgraphutil -I../libgraphcore -I../../utility/libutil
CXXFLAGS += -I../libgraphutil -I../libgraphcore -I../../utility/libutil

TESTLIBS += ../libgraphutil/libgraphutil.a ../libgraphcore/libgraphcore.a

libgraphalgo.a: $(subst .cc,.o,$(libgraphalgo_sources))
	$(make-library)

quick_zykov_test:	quick_zykov_test.o $(libraries) $(TESTLIBS)
bron1_test:	bron1_test.o $(libraries) $(TESTLIBS)
bron2_test:	bron2_test.o $(libraries) $(TESTLIBS)

-include $(subst .cc,.d,$(sources))
