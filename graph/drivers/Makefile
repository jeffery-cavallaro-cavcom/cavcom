includes :=

edwards_sources := edwards.cc
bron_sources := bron.cc
greedylf_sources := greedylf.cc
greedylfint_sources := greedylfint.cc
cf_source := cf.cc
wang_source := wang.cc
znone_source := znone.cc
qz_sources := qz.cc

test_sources := \
	$(edwards_sources) \
	$(bron_sources) \
	$(greedylf_sources) \
	$(greedylfint_sources) \
	$(cf_sources) \
	$(wang_sources) \
	$(znone_sources) \
	$(qz_sources)

sources := $(test_sources)

libraries :=

programs := edwards bron greedylf greedylfint cf wang znone qz

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
greedylfint:	greedylfint.o $(LIBS)
cf:	cf.o $(LIBS)
wang:	wang.o $(LIBS)
znone:	znone.o $(LIBS)
qz:	qz.o $(LIBS)

-include $(subst .cc,.d,$(sources))
