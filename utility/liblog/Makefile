includes := datum.h counter.h outfile.h

liblog_sources := outfile.cc

test_sources :=	datum_test.cc counter_test.cc outfile_test.cc

sources := $(liblog_sources) $(test_sources)

libraries := liblog.a

tests := datum_test counter_test outfile_test

include ../../templates/Makefile.all.mk

CPPFLAGS += --std=c++17 -I../../utility/libutil
CXXFLAGS += --std=c++17 -I../../utility/libutil

TESTLIBS += liblog.a

LDLIBS += -lstdc++fs

liblog.a: $(subst .cc,.o,$(liblog_sources))
	$(make-library)

datum_test:	datum_test.o $(libraries) $(TESTLIBS)
counter_test:	counter_test.o $(libraries) $(TESTLIBS)
outfile_test:	outfile_test.o $(libraries) $(TESTLIBS)

-include $(subst .cc,.d,$(sources))
