includes := \
	lookup.h \
	matrix.h \
	types.h

sources := \
	lookup_test.cc \
	matrix_test.cc \
	types_test.cc

tests := \
	lookup_test \
	matrix_test \
	types_test

include ../../templates/Makefile.all.mk

lookup_test:	lookup_test.o $(TESTLIBS)
matrix_test:	matrix_test.o $(TESTLIBS)
types_test:	types_test.o $(TESTLIBS)

-include $(subst .cc,.d,$(sources))
