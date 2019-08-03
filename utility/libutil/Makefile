includes := types.h

sources := types_test.cc

tests := types_test

include ../../templates/Makefile.all.mk

types_test:	types_test.o -lunittest -lpthread

-include $(subst .cc,.d,$(sources))
