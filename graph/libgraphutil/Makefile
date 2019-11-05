includes := formatter.h tikzformatter.h

libgraphutil_sources := tikzformatter.cc

test_sources := 

sources := $(libgraphutil_sources) $(test_sources)

libraries := libgraphutil.a

tests := 

include ../../templates/Makefile.all.mk

CPPFLAGS += -I../libgraph -I../../utility/libutil
CXXFLAGS += -I../libgraph -I../../utility/libutil

TESTLIBS += ../libgraph/libgraph.a

libgraphutil.a: $(subst .cc,.o,$(libgraphutil_sources))
	$(make-library)

#tikz_test:	tikz_test.o $(libraries) $(TESTLIBS)

-include $(subst .cc,.d,$(sources))
