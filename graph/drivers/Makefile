includes :=

qz_sources := qz.cc
bron_sources := bron.cc
cf_source := cf.cc

test_sources := 

sources := $(qz_sources) $(bron_sources) $(cf_sources) $(test_sources)

libraries :=

programs := qz bron cf

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
bron:	bron.o $(LIBS)
cf:	cf.o $(LIBS)

-include $(subst .cc,.d,$(sources))
