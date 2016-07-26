all: libccan.a

include Makefile-ccan
tools/configurator/configurator: tools/configurator/configurator.c

config.h: tools/configurator/configurator Makefile Makefile-ccan
	tools/configurator/configurator $(CC) $(CCAN_CFLAGS) > $@ \
		|| rm -f $@

objs = $(patsubst %.c, %.o, $(wildcard ccan/*/*.c))
$(objs): config.h

.PHONY: clean
clean:
	rm -f `find * -name '*.o'` `find * -name '.depends'` `find * -name '*.a'`  `find * -name info` `find * -name '*.d'` `find ccan -name '*-Makefile'` `find * -name 'info'`
	rm -f config.h
