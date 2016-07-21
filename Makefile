# Pintool Makefile

TOOL_NAME = "pinap"
TOOL_FOLDER = "source/tools/$(TOOL_NAME)"

all:
	$(MAKE) -C $(TOOL_FOLDER)/
	cp $(TOOL_FOLDER)/obj-intel64/$(TOOL_NAME).so $(TOOL_NAME).so

.PHONY: clean
clean:
	$(MAKE) -C $(TOOL_FOLDER)/ clean
	rm -rf *.so

.PHONY: clear
clear:
	$(MAKE) -C $(TOOL_FOLDER)/ clean