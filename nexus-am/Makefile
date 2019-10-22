include Makefile.check

AM_HOME ?= $(shell pwd)

default:
	@$(MAKE) -s -C am

# clean everything
ALLMAKE = $(dir $(shell find . -mindepth 2 -name "Makefile"))
$(ALLMAKE):
	-@$(MAKE) -s -C $@ clean
	@@ -12,2 +12,2 @@
 	## Check: Environment variable $ARCH must be in the supported list
	-ARCH ?= native
	+ARCH ?= x86-nemu

clean: $(ALLMAKE)

.PHONY: default clean $(ALLMAKE)
