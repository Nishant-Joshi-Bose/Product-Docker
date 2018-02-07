export BOSE_WORKSPACE := $(abspath $(CURDIR))
include Settings.mk

.PHONY: default
default: graph

.PHONY: force
force:

VERSION_FILES = \
 $(BUILDS_DIR)/BoseVersion.h \
 $(BUILDS_DIR)/BoseVersion.json \

$(VERSION_FILES): version.txt | $(BUILDS_DIR)
	gen-version-files version.txt $(BUILDS_DIR)

ifndef DONT_UPDATE_VERSION
$(VERSION_FILES): force
endif

$(BUILDS_DIR):
	mkdir -p $@

.PHONY: check_tools
check_tools:
ifndef DONT_UPDATE_CASTLETOOLS
	castletools-update
endif
	components install

CMAKE_USE_CCACHE := $(USE_CCACHE)

A4VVIDEOMANAGERSERVICE_DIR = $(shell components get A4VVideoManagerService installed_location)
A4VQUICKSETSERVICE_DIR = $(shell components get A4VQuickSetService installed_location)
A4VREMOTECOMMUNICATIONSERVICE_DIR = $(shell components get A4VRemoteCommunicationService installed_location)
PROFESSORLPMPACKAGE_DIR = $(shell components get ProfessorLPM-Package installed_location)
PRODUCTCONTROLLERCOMMON_DIR = $(shell components get CastleProductControllerCommon installed_location)
RIVIERALPMUPDATER_DIR = $(shell components get RivieraLpmUpdater installed_location)
SOFTWARE_UPDATE_DIR = $(shell components get SoftwareUpdate-qc8017_32 installed_location)
RIVIERALPM_DIR = $(shell components get RivieraLPM installed_location)

.PHONY: generated_sources
generated_sources: check_tools $(VERSION_FILES)
	$(MAKE) -C ProductController $@
	$(MAKE) -C $(PRODUCTCONTROLLERCOMMON_DIR) $@
	$(MAKE) -C $(A4VVIDEOMANAGERSERVICE_DIR) $@
	$(MAKE) -C $(A4VQUICKSETSERVICE_DIR) $@
	$(MAKE) -C $(A4VREMOTECOMMUNICATIONSERVICE_DIR) $@

.PHONY: astyle
astyle:
ifndef DONT_RUN_ASTYLE
	run-astyle
endif

USERKEYCONFIG=$(PWD)/Config/UserKeyConfig.json
KEYCONFIG=$(PWD)/opt-bose-fs/etc/KeyConfiguration.json
LPM_KEYS=$(RIVIERALPM_DIR)/include/RivieraLPM_KeyValues.h
INTENT_DEFS=$(PWD)/ProductController/source/IntentHandler/Intents.h 
KEYCONFIG_INCS=$(PRODUCTCONTROLLERCOMMON_DIR)/IntentHandler

.PHONY: keyconfig
keyconfig:
	cd tools/key_config_generator && \
	./generate_key_config \
		$(BUILDS_DIR) \
		--inputcfg $(USERKEYCONFIG) \
		--actions $(INTENT_DEFS) \
		--cap $(LPM_KEYS) \
		--ir $(LPM_KEYS) \
		--tap $(LPM_KEYS) \
		--cec $(LPM_KEYS) \
		--rf $(LPM_KEYS) \
		--outputcfg $(KEYCONFIG) \
		--incdirs $(KEYCONFIG_INCS)

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
#PACKAGENAMES = hsp SoundTouch lpm_updater
IPKS = monaco.ipk product.ipk lpm_updater.ipk
PACKAGENAMES = monaco SoundTouch lpm_updater

#Create Zip file for Bonjour / Local update
.PHONY: update-zip
update-zip: monaco-ipk product-ipk hsp-ipk lpmupdater-ipk
	cd $(BOSE_WORKSPACE)/builds/$(cfg) && python2.7 $(SOFTWARE_UPDATE_DIR)/make-update-zip.py -n $(PACKAGENAMES) -i $(IPKS) -s $(BOSE_WORKSPACE)/builds/$(cfg) -d $(BOSE_WORKSPACE)/builds/$(cfg) -o product_update.zip -k $(privateKeyFilePath) -p $(privateKeyPasswordPath)

#Create one more Zip file for Bonjour / Local update with HSP 
#- This is temporary, till DP2 boards are available.
IPKS_HSP = hsp.ipk monaco.ipk product.ipk lpm_updater.ipk
PACKAGENAMES_HSP = hsp monaco SoundTouch lpm_updater
.PHONY: update-zip-with-hsp
update-zip-with-hsp: monaco-ipk product-ipk hsp-ipk lpmupdater-ipk
	cd $(BOSE_WORKSPACE)/builds/$(cfg) && python2.7 $(SOFTWARE_UPDATE_DIR)/make-update-zip.py -n $(PACKAGENAMES_HSP) -i $(IPKS_HSP) -s $(BOSE_WORKSPACE)/builds/$(cfg) -d $(BOSE_WORKSPACE)/builds/$(cfg) -o product_update_with_hsp.zip -k $(privateKeyFilePath) -p $(privateKeyPasswordPath)

.PHONY: packages-gz
packages-gz: monaco-ipk product-ipk hsp-ipk lpmupdater-ipk
	cd $(BOSE_WORKSPACE)/builds/$(cfg) && $(SOFTWARE_UPDATE_DIR)/make-packages-gz.sh Packages.gz $(IPKS)

.PHONY: packages-gz-with-hsp
packages-gz-with-hsp: monaco-ipk product-ipk hsp-ipk lpmupdater-ipk
	cd $(BOSE_WORKSPACE)/builds/$(cfg) && $(SOFTWARE_UPDATE_DIR)/make-packages-gz.sh Packages.gz $(IPKS_HSP)

.PHONY: graph
graph: product-ipk
	graph-components --sdk=$(sdk) Professor builds/$(cfg)/product-ipk-stage/component-info.gz >builds/$(cfg)/components.dot
	dot -Tsvgz builds/$(cfg)/components.dot -o builds/$(cfg)/components.svgz

.PHONY: hsp-ipk
hsp-ipk: cmake_build
	./scripts/create-hsp-ipk

.PHONY: lpmupdater-ipk
lpmupdater-ipk:
	$(RIVIERALPMUPDATER_DIR)/create-ipk $(RIVIERALPMUPDATER_DIR)/lpm-updater-ipk-stage $(PROFESSORLPMPACKAGE_DIR) ./builds/$(cfg)/ professor

.PHONY: monaco-ipk
monaco-ipk:
	./scripts/create-monaco-ipk

.PHONY: package
package: product-ipk hsp-ipk lpmupdater-ipk monaco-ipk
	./scripts/create-product-tar

.PHONY: all-packages
all-packages: package packages-gz-with-hsp update-zip-with-hsp packages-gz update-zip

.PHONY: deploy
deploy: graph all-packages
ifndef RIVIERA_HSP_VERSION
	$(error No RIVIERA_HSP_VERSION)
endif
	scripts/collect-deployables builds/Release builds/deploy/HSP-${RIVIERA_HSP_VERSION}

.PHONY: clean
clean:
	rm -rf $(BUILDS_DIR) builds/CastleTestUtils builds/__init__.py
	find . -name \*.pyc -delete

.PHONY: distclean
distclean:
	git clean -fdX
