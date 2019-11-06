includes := formatter.h tikzformatter.h

libgraphutil_sources := tikzformatter.cc

test_sources := formatter_test.cc tikz_formatter_test.cc

sources := $(libgraphutil_sources) $(test_sources)

libraries := libgraphutil.a

tests := formatter_test tikz_formatter_test

include ../../templates/Makefile.all.mk

CPPFLAGS += -I../libgraph -I../../utility/libutil
CXXFLAGS += -I../libgraph -I../../utility/libutil

TESTLIBS += ../libgraph/libgraph.a

libgraphutil.a: $(subst .cc,.o,$(libgraphutil_sources))
	$(make-library)

formatter_test:	formatter_test.o $(libraries) $(TESTLIBS)
tikz_formatter_test:	tikz_formatter_test.o $(libraries) $(TESTLIBS)

-include $(subst .cc,.d,$(sources))
