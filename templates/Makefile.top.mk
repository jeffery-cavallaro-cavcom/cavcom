# Top Level Project Makefile

.PHONY:	all $(targets) lint clean distclean check

all:
	for d in $(targets); do $(MAKE) --directory=$$d all; done

$(targets):
	$(MAKE) --directory=$@

check:
	for d in $(targets); do $(MAKE) --directory=$$d check; done

clean:
	for d in $(targets); do $(MAKE) --directory=$$d clean; done

distclean:
	for d in $(targets); do $(MAKE) --directory=$$d distclean; done

help:
	for d in $(targets); do $(MAKE) --directory=$$d help; done

lint:
	for d in $(targets); do $(MAKE) --directory=$$d lint; done
