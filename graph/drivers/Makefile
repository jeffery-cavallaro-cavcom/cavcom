includes :=

edwards_sources := edwards.cc
bron_sources := bron.cc
greedylf_sources := greedylf.cc
cf_source := cf.cc
qz_sources := qz.cc

test_sources := $(edwards_sources) $(bron_sources) $(greedylf_sources) $(cf_sources) $(qz_sources)

sources := $(test_sources)

libraries :=

programs := edwards bron greedylf cf qz

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

edwards:	edwards.o $(LIBS)
bron:	bron.o $(LIBS)
greedylf:	greedylf.o $(LIBS)
cf:	cf.o $(LIBS)
qz:	qz.o $(LIBS)

-include $(subst .cc,.d,$(sources))
