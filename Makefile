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

CMAKE_USE_CCACHE := $(USE_CCACHE)

EDDIELPMPACKAGE_DIR = $(shell components get EddieLPM-Package installed_location)
PRODUCTCONTROLLERCOMMON_DIR = $(shell components get CastleProductControllerCommon installed_location)
RIVIERALPMUPDATER_DIR = $(shell components get RivieraLpmUpdater installed_location)
SOFTWARE_UPDATE_DIR = $(shell components get SoftwareUpdate-qc8017_32 installed_location)
TESTUTILS_DIR = $(shell components get CastleTestUtils installed_location)

.PHONY: generated_sources
generated_sources: check_tools version-files
	$(MAKE) -C ProductController $@
	$(MAKE) -C $(PRODUCTCONTROLLERCOMMON_DIR) $@
	ln -nsf $(TESTUTILS_DIR) builds/CastleTestUtils
	touch builds/__init__.py

.PHONY: astyle
astyle:
ifndef DONT_RUN_ASTYLE
	run-astyle
endif

.PHONY: cmake_build
cmake_build: generated_sources | $(BUILDS_DIR) astyle
	rm -rf $(BUILDS_DIR)/CMakeCache.txt $(BUILDS_DIR)/CMakeFiles
	cd $(BUILDS_DIR) && cmake -DCFG=$(cfg) -DSDK=$(sdk) $(CURDIR) -DUSE_CCACHE=$(CMAKE_USE_CCACHE)
	$(MAKE) -C $(BUILDS_DIR) -j $(jobs) install

.PHONY: product-ipk
product-ipk: cmake_build
	./scripts/create-product-ipk

#Uncomment next two line after removing next 2 lines, once HSP is integrated.
#IPKS = hsp.ipk  product.ipk lpm_updater.ipk
#PACKAGENAMES = hsp SoundTouch eddie_lpm_updater
IPKS = monaco.ipk product.ipk lpm_updater.ipk
PACKAGENAMES = monaco SoundTouch eddie_lpm_updater

#Create Zip file for Bonjour / Local update
.PHONY: update-zip
update-zip: monaco-ipk product-ipk hsp-ipk lpmupdater-ipk
	cd $(BOSE_WORKSPACE)/builds/$(cfg) && python2.7 $(SOFTWARE_UPDATE_DIR)/make-update-zip.py -n $(PACKAGENAMES) -i $(IPKS) -s $(BOSE_WORKSPACE)/builds/$(cfg) -d $(BOSE_WORKSPACE)/builds/$(cfg) -o product_update.zip

#Create one more Zip file for Bonjour / Local update with HSP 
#- This is temporary, till DP2 boards are not available.
IPKS_HSP = hsp.ipk monaco.ipk product.ipk lpm_updater.ipk
PACKAGENAMES_HSP = hsp monaco SoundTouch eddie_lpm_updater
.PHONY: update-zip-with-hsp
update-zip-with-hsp: monaco-ipk product-ipk hsp-ipk lpmupdater-ipk
	cd $(BOSE_WORKSPACE)/builds/$(cfg) && python2.7 $(SOFTWARE_UPDATE_DIR)/make-update-zip.py -n $(PACKAGENAMES_HSP) -i $(IPKS_HSP) -s $(BOSE_WORKSPACE)/builds/$(cfg) -d $(BOSE_WORKSPACE)/builds/$(cfg) -o product_update_with_hsp.zip

.PHONY: packages-gz
packages-gz: monaco-ipk product-ipk hsp-ipk lpmupdater-ipk
	cd $(BOSE_WORKSPACE)/builds/$(cfg) && $(SOFTWARE_UPDATE_DIR)/make-packages-gz.sh Packages.gz $(IPKS)

.PHONY: graph
graph: product-ipk
	graph-components --sdk=$(sdk) Eddie builds/$(cfg)/product-ipk-stage/component-info.gz >builds/$(cfg)/components.dot
	dot -Tsvgz builds/$(cfg)/components.dot -o builds/$(cfg)/components.svgz

.PHONY: hsp-ipk
hsp-ipk: cmake_build
	./scripts/create-hsp-ipk

.PHONY: lpmupdater-ipk
lpmupdater-ipk:
	$(RIVIERALPMUPDATER_DIR)/create-ipk $(RIVIERALPMUPDATER_DIR)/lpm-updater-ipk-stage $(EDDIELPMPACKAGE_DIR) ./builds/$(cfg)/ eddie

.PHONY: monaco-ipk
monaco-ipk:
	./scripts/create-monaco-ipk

.PHONY: package
package: product-ipk hsp-ipk lpmupdater-ipk monaco-ipk
	./scripts/create-product-tarball

.PHONY: all-packages
all-packages: package packages-gz update-zip update-zip-with-hsp

.PHONY: clean
clean:
	rm -rf $(BUILDS_DIR) builds/CastleTestUtils builds/__init__.py
	find . -name \*.pyc -delete

.PHONY: distclean
distclean:
	git clean -fdX
