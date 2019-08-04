includes := \
	matrix.h \
	matrix_base.h \
	types.h

sources := \
	matrix_test.cc \
	types_test.cc

tests := \
	matrix_test \
	types_test

include ../../templates/Makefile.all.mk

matrix_test:	matrix_test.o $(TESTLIBS)
types_test:	types_test.o $(TESTLIBS)

-include $(subst .cc,.d,$(sources))
