export BOSE_WORKSPACE := $(abspath $(CURDIR))
include Settings.mk

.PHONY: default
ifeq ($(sdk),native)
default: cmake_build
else
default: graph
endif

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
CASTLEPRODUCTCONTROLLERCOMMON_DIR = $(shell components get CastleProductControllerCommon installed_location)
RIVIERALPMUPDATER_DIR = $(shell components get RivieraLpmUpdater installed_location)
A4VVIDEOMANAGERSERVICE_DIR = $(shell components get A4VVideoManagerService installed_location)
#A4VREMOTECOMMUNICATIONSERVICE_DIR = $(shell components get A4VRemoteCommunicationService installed_location)

.PHONY: generated_sources
generated_sources: check_tools version-files
	$(MAKE) -C ProductController $@
	$(MAKE) -C $(RIVIERALPMSERVICE_DIR) $@
	$(MAKE) -C $(CASTLEPRODUCTCONTROLLERCOMMON_DIR) $@
	$(MAKE) -C $(RIVIERALPMUPDATER_DIR) $@
	$(MAKE) -C $(A4VVIDEOMANAGERSERVICE_DIR) $@
#	$(MAKE) -C $(A4VREMOTECOMMUNICATIONSERVICE_DIR) $@

.PHONY: astyle
astyle:
ifndef DONT_RUN_ASTYLE
	run-astyle
endif

.PHONY: cmake_build
cmake_build: generated_sources | $(BUILDS_DIR) astyle
	rm -rf $(BUILDS_DIR)/CMakeCache.txt $(BUILDS_DIR)/CMakeFiles
# Symlinks to placate cmake's add_subdirectory which doesn't like absolute paths.
	ln -nsf $(RIVIERALPMSERVICE_DIR) builds/RivieraLpmService
	ln -nsf $(CASTLEPRODUCTCONTROLLERCOMMON_DIR) builds/CastleProductControllerCommon
	ln -nsf $(RIVIERALPMUPDATER_DIR) builds/RivieraLpmUpdater
	ln -nsf $(A4VVIDEOMANAGERSERVICE_DIR) builds/A4VVideoManagerService
#	ln -nsf $(A4VREMOTECOMMUNICATIONSERVICE_DIR) builds/A4VRemoteCommunicationService
	cd $(BUILDS_DIR) && cmake -DCFG=$(cfg) -DSDK=$(sdk) $(CURDIR)
	$(MAKE) -C $(BUILDS_DIR) -j $(jobs) install

.PHONY: product-ipk
product-ipk: cmake_build
	./scripts/create-product-ipk

.PHONY: graph
graph: product-ipk
	graph-components --exclude='-(native|ti)$$' Professor builds/$(cfg)/product-ipk-stage/component-info >builds/$(cfg)/components.dot
	dot -Tsvgz builds/$(cfg)/components.dot -o builds/$(cfg)/components.svgz

.PHONY: package
package: product-ipk
	./scripts/create-product-tarball

.PHONY: clean
clean:
	rm -rf $(BUILDS_DIR)

.PHONY: distclean
distclean:
	git clean -fdX
