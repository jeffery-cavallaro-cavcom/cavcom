includes := quick_zykov.h

libgraphalgo_sources := quick_zykov.cc

test_sources := quick_zykov_test.cc

sources := $(libgraphalgo_sources) $(test_sources)

libraries := libgraphalgo.a

tests := quick_zykov_test

include ../../templates/Makefile.all.mk

CPPFLAGS += -I../libgraphutil -I../libgraphcore -I../../utility/libutil
CXXFLAGS += -I../libgraphutil -I../libgraphcore -I../../utility/libutil

TESTLIBS += ../libgraphutil/libgraphutil.a ../libgraphcore/libgraphcore.a

libgraphalgo.a: $(subst .cc,.o,$(libgraphalgo_sources))
	$(make-library)

quick_zykov_test:	quick_zykov_test.o $(libraries) $(TESTLIBS)

-include $(subst .cc,.d,$(sources))
