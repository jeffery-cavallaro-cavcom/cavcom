includes :=	datum.h \
		sum.h \
		percent.h \
		hit_counter.h \
		sample.h \
		out_file.h \
		csv_field.h \
		csv_hit_counter_fields.h \
		csv_sample_fields.h \
		csv_file.h

liblog_sources := hit_counter.cc out_file.cc csv_hit_counter_fields.cc csv_file.cc

test_sources :=	datum_test.cc \
		sum_test.cc \
		percent_test.cc \
		hit_counter_test.cc \
		sample_test.cc \
		out_file_test.cc \
		csv_file_test.cc

sources := $(liblog_sources) $(test_sources)

libraries := liblog.a

tests := datum_test sum_test percent_test hit_counter_test sample_test out_file_test csv_file_test

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
out_file_test:	out_file_test.o $(libraries) $(TESTLIBS)
csv_file_test:	csv_file_test.o $(libraries) $(TESTLIBS)

-include $(subst .cc,.d,$(sources))
