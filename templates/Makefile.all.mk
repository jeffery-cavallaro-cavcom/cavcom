# Expected Symbols:
#
#    programs
#    libraries
#    includes
#    sources
#    tests

CC := g++

CXX := g++

CXXFLAGS := -g -Wall

TESTLIBS := -lunittest -lpthread

LINT := cpplint

define make-library
	$(AR) $(ARFLAGS) $@ $?
endef

define show-help
	@$(MAKE) --print-data-base --question msg | \
	grep -v -e '^no-such-target' -e '^Makefile' | \
	awk '/^[^.%][-A-Za-z0-9_]*:/ { print substr($$1, 1, length($$1)-1) }' | \
	sort | \
	pr --omit-pagination --width=80 --columns=4
endef

define d-from-cc
	@$(CC) -M $(CPPFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	$(RM) -f $@.$$$$
endef

.PHONY:	all check clean distclean help lint

all:	msg $(libraries) $(programs)

msg:	$(includes) $(sources)
	@echo "Changed: $?"
	@touch $@

check: all $(tests)
	@for d in $(tests); do $$d -v; done

clean:
	@$(RM) -f $(programs) $(tests) *.a *.o

distclean: clean
	@$(RM) -f msg *.d

help:
	@$(show-help)

lint:
	@$(LINT) --root=../../.. $(includes) $(sources)

%.d: %.cc
	@$(d-from-cc)
