export BOSE_WORKSPACE := $(abspath $(CURDIR))
include Settings.mk

.PHONY: default
default: product-ipk

.PHONY: version-files
version-files: | $(BUILDS_DIR)
	gen-version-files version.txt $(BUILDS_DIR)

$(BUILDS_DIR):
	mkdir -p $@

.PHONY: check_tools
check_tools:
ifndef DONT_UPDATE_CASTLETOOLS
	castletools-update
endif
	components install

RIVIERALPMSERVICE_DIR = $(shell components get RivieraLpmService installed_location)

.PHONY: generated_sources
generated_sources: check_tools version-files
	$(MAKE) -C ProductController $@
	$(MAKE) -C $(RIVIERALPMSERVICE_DIR) $@

.PHONY: cmake_build
cmake_build: generated_sources | $(BUILDS_DIR)
	rm -fv $(BUILDS_DIR)/CMakeCache.txt
# Symlink to placate cmake's add_directory which doesn't like absolute paths.
	ln -nsf $(RIVIERALPMSERVICE_DIR) builds/RivieraLpmService
	cd $(BUILDS_DIR) && cmake -DCFG=$(cfg) -DSDK=$(sdk) $(CURDIR)
	$(MAKE) -C $(BUILDS_DIR) -j $(jobs) install

.PHONY: product-ipk
product-ipk: cmake_build
	./scripts/create-product-ipk

.PHONY: package
package: product-ipk
	./scripts/create-package

.PHONY: clean
clean:
	rm -rf $(BUILDS_DIR)

.PHONY: distclean
distclean:
	git clean -fdX
