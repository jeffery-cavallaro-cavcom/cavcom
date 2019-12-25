includes := \
	container_formatter.h \
	lookup.h \
	matrix.h \
	types.h

libutil_sources := container_formatter.cc

test_sources := \
	container_formatter_test.cc \
	lookup_test.cc \
	matrix_test.cc \
	types_test.cc

source := $(libutil_source) $(test_sources)

libraries := libutil.a

tests := \
	container_formatter_test \
	lookup_test \
	matrix_test \
	types_test

include ../../templates/Makefile.all.mk

TESTLIBS += libutil.a

libutil.a: $(subst .cc,.o,$(libutil_sources))
	$(make-library)

container_formatter_test:	container_formatter_test.o $(TESTLIBS)
lookup_test:	lookup_test.o $(TESTLIBS)
matrix_test:	matrix_test.o $(TESTLIBS)
types_test:	types_test.o $(TESTLIBS)

-include $(subst .cc,.d,$(sources))
