all:
	$(MAKE) -C source/tools/MyPinTool/
	cp source/tools/MyPinTool/obj-intel64/MyPinTool.so MyPinTool.so

.PHONY: clean
clean:
	$(MAKE) -C source/tools/MyPinTool/ clean
	rm -rf *.so

.PHONY: clear
clear:
	$(MAKE) -C source/tools/MyPinTool/ clean