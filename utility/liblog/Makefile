includes := datum.h sum.h percent.h hit_counter.h sample.h outfile.h

liblog_sources := hit_counter.cc outfile.cc

test_sources :=	datum_test.cc sum_test.cc percent_test.cc hit_counter_test.cc sample_test.cc outfile_test.cc

sources := $(liblog_sources) $(test_sources)

libraries := liblog.a

tests := datum_test sum_test percent_test hit_counter_test sample_test outfile_test

include ../../templates/Makefile.all.mk

CPPFLAGS += --std=c++17 -I../../utility/libutil
CXXFLAGS += --std=c++17 -I../../utility/libutil

TESTLIBS += liblog.a

LDLIBS += -lstdc++fs

liblog.a: $(subst .cc,.o,$(liblog_sources))
	$(make-library)

datum_test:	datum_test.o $(libraries) $(TESTLIBS)
sum_test:	sum_test.o $(libraries) $(TESTLIBS)
percent_test:	percent_test.o $(libraries) $(TESTLIBS)
hit_counter_test:	hit_counter_test.o $(libraries) $(TESTLIBS)
sample_test:	sample_test.o $(libraries) $(TESTLIBS)
outfile_test:	outfile_test.o $(libraries) $(TESTLIBS)

-include $(subst .cc,.d,$(sources))
