includes :=

qz_sources := qz.cc
b1_sources := b1.cc

test_sources := 

sources := $(qz_sources) $(b1_sources) $(test_sources)

libraries :=

programs := qz b1

tests := 

include ../../templates/Makefile.all.mk

CPPFLAGS +=	--std=c++17 \
		-I../libgraphalgo \
		-I../libgraphutil \
		-I../libgraphcore \
		-I../../utility/liblog \
		-I../../utility/libutil

CXXFLAGS = ${CPPFLAGS}

LIBS +=	../libgraphalgo/libgraphalgo.a \
	../libgraphutil/libgraphutil.a \
	../libgraphcore/libgraphcore.a \
	../../utility/liblog/liblog.a

LDLIBS += -lstdc++fs

qz:	qz.o $(LIBS)
b1:	b1.o $(LIBS)

-include $(subst .cc,.d,$(sources))
