includes :=

qz_sources := qz.cc

test_sources := 

sources := $(qz_sources) $(test_sources)

libraries :=

programs := qz

tests := 

include ../../templates/Makefile.all.mk

CPPFLAGS += -I../libgraphalgo -I../libgraphutil -I../libgraphcore -I../../utility/libutil
CXXFLAGS += -I../libgraphalgo -I../libgraphutil -I../libgraphcore -I../../utility/libutil

LIBS += ../libgraphalgo/libgraphalgo.a ../libgraphutil/libgraphutil.a ../libgraphcore/libgraphcore.a

qz:	qz.o $(LIBS)

-include $(subst .cc,.d,$(sources))
