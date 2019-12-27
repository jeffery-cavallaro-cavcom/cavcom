includes :=

edwards_sources := edwards.cc
bron_sources := bron.cc
greedyerr_sources := greedyerr.cc
greedyrun_sources := greedyrun.cc
greedyinterr_sources := greedyinterr.cc
greedyintrun_sources := greedyintrun.cc
cf_source := cf.cc
wang_source := wang.cc
zykov_source := zykov.cc
qz_sources := qz.cc

test_sources := \
	$(edwards_sources) \
	$(bron_sources) \
	$(greedyerr_sources) \
	$(greedyrun_sources) \
	$(greedyinterr_sources) \
	$(greedyintrun_sources) \
	$(cf_sources) \
	$(wang_sources) \
	$(zykov_sources) \
	$(qz_sources)

sources := $(test_sources)

libraries :=

programs := edwards bron greedyerr greedyrun greedyinterr greedyintrun cf wang zykov qz

tests := 

include ../../templates/Makefile.all.mk

CPPFLAGS +=	--std=c++17 \
		-I../libgraphalgo \
		-I../libgraphutil \
		-I../libgraphcore \
		-I../../utility/liblog \
		-I../../utility/libutil

CXXFLAGS += ${CPPFLAGS}

LIBS +=	../libgraphalgo/libgraphalgo.a \
	../libgraphutil/libgraphutil.a \
	../libgraphcore/libgraphcore.a \
	../../utility/liblog/liblog.a \
	../../utility/libutil/libutil.a

LDLIBS += -lstdc++fs

edwards:	edwards.o $(LIBS)
bron:	bron.o $(LIBS)
greedyerr:	greedyerr.o $(LIBS)
greedyrun:	greedyrun.o $(LIBS)
greedyinterr:	greedyinterr.o $(LIBS)
greedyintrun:	greedyintrun.o $(LIBS)
cf:	cf.o $(LIBS)
wang:	wang.o $(LIBS)
zykov:	zykov.o $(LIBS)
qz:	qz.o $(LIBS)

-include $(subst .cc,.d,$(sources))
