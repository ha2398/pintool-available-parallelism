# Pintool Makefile

CCAN_FOLDER = ccan
CCAN_CFLAGS = -O2 -fPIC
TOOL_NAME = pinap
TOOL_FOLDER = source/tools/$(TOOL_NAME)

.PHONY: all
all: ccan pinap

.PHONY: ccan
ccan:
	$(MAKE) -C $(CCAN_FOLDER)/ CCAN_CFLAGS="$(CCAN_CFLAGS)"

.PHONY: pinap
pinap:
	$(MAKE) -C $(TOOL_FOLDER)/ && \
	cp $(TOOL_FOLDER)/obj-intel64/$(TOOL_NAME).so $(TOOL_NAME).so

.PHONY: clear
clear:
	$(MAKE) -C $(CCAN_FOLDER)/ clean
	$(MAKE) -C $(TOOL_FOLDER)/ clean

.PHONY: clean
clean: clear
	rm -rf *.so *.out
