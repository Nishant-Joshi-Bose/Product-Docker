export BOSE_WORKSPACE := $(abspath $(CURDIR))
include Settings.mk

ifneq ($(filter $(HW_VAR), DP2 Alpha),$(HW_VAR))
	$(error HW_VAR must equal DP2 or Alpha. Found $(HW_VAR))
endif

.PHONY: deploy
deploy: all-packages
	scripts/collect-deployables . builds/Release builds/deploy/$(HW_VAR)

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
	castletools-build-host-is-sane

CMAKE_USE_CCACHE := $(USE_CCACHE)

RIVIERALPM_DIR = $(shell components get RivieraLPM installed_location)
RIVIERA_LPM_TOOLS_DIR = $(shell components get RivieraLpmTools installed_location)
PRODUCTCONTROLLERCOMMON_DIR = $(shell components get ProductControllerCommon installed_location)
RIVIERALPMUPDATER_DIR = $(shell components get RivieraLpmUpdater installed_location)
SOFTWARE_UPDATE_DIR = $(shell components get SoftwareUpdate-qc8017_32 installed_location)
TESTUTILS_DIR = $(shell components get TestUtils installed_location)
PRODUCTCONTROLLERCOMMONPROTO_DIR = $(shell components get ProductControllerCommonProto-qc8017_32 installed_location)
RIVIERALPMSERVICE_DIR = $(shell components get RivieraLpmService-qc8017_32 installed_location)

.PHONY: generated_sources
generated_sources: check_tools $(VERSION_FILES)
	$(MAKE) -C ProductController $@
	$(MAKE) -C $(PRODUCTCONTROLLERCOMMON_DIR) $@
	ln -nsf $(TESTUTILS_DIR) builds/CastleTestUtils
	touch builds/__init__.py

.PHONY: astyle
astyle:
ifndef DONT_RUN_ASTYLE
	run-astyle
endif


KEYCONFIG_GENERATOR_DIR=$(PRODUCTCONTROLLERCOMMON_DIR)/tools/key_config_generator

USERKEYCONFIG=$(BOSE_WORKSPACE)/opt-bose-fs/etc/UserKeyConfig.json
COMMON_INTENTS=$(BOSE_WORKSPACE)/builds/$(cfg)/$(sdk)/proto_py/CommonIntents_pb2.py
CUSTOM_INTENTS=$(BOSE_WORKSPACE)/builds/$(cfg)/$(sdk)/proto_py/Intents_pb2.py
AUTOLPM_SERVICES=$(RIVIERALPMSERVICE_DIR)/Python/AutoLpmServiceMessages_pb2.py

KEYCONFIG=$(BOSE_WORKSPACE)/builds/$(cfg)/$(sdk)/KeyConfiguration.json
BLASTCONFIG=$(BOSE_WORKSPACE)/builds/$(cfg)/$(sdk)/BlastConfiguration.json

$(COMMON_INTENTS) $(CUSTOM_INTENTS) $(AUTOLPM_SERVICES): | generated_sources

$(KEYCONFIG): $(USERKEYCONFIG) $(AUTOLPM_SERVICES) $(CUSTOM_INTENTS) $(COMMON_INTENTS)
	cd $(KEYCONFIG_GENERATOR_DIR) && \
	./generate_key_config \
		$(BUILDS_DIR) \
		--inputcfg $(USERKEYCONFIG) \
		--common $(COMMON_INTENTS) \
		--custom $(CUSTOM_INTENTS) \
		--autolpm $(AUTOLPM_SERVICES) \
		--keys $(AUTOLPM_SERVICES) \
		--outputcfg $(KEYCONFIG)

$(BLASTCONFIG): $(USERKEYCONFIG) $(AUTOLPM_SERVICES)
	cd tools/key_config_generator && \
	./generate_blast_config \
		$(BUILDS_DIR) \
		--inputcfg $(USERKEYCONFIG) \
		--keys $(AUTOLPM_SERVICES) \
		--outputcfg $(BLASTCONFIG)

.PHONY: cmake_build
cmake_build: generated_sources $(BLASTCONFIG) $(KEYCONFIG)| $(BUILDS_DIR) astyle
	rm -rf $(BUILDS_DIR)/CMakeCache.txt
	cd $(BUILDS_DIR) && cmake -DCFG=$(cfg) -DSDK=$(sdk) $(CURDIR) -DUSE_CCACHE=$(CMAKE_USE_CCACHE)
	$(MAKE) -C $(BUILDS_DIR) -j $(jobs) install

.PHONY: minimal-product-tar
minimal-product-tar: cmake_build
	./scripts/create-minimal-product-tar professor

.PHONY: product-ipk
product-ipk: cmake_build
	./scripts/create-product-ipk

# The default build will always be signed using development keys
privateKeyFilePath = $(BOSE_WORKSPACE)/keys/development/privateKey/dev.p12
privateKeyPasswordPath = $(BOSE_WORKSPACE)/keys/development/privateKey/dev_p12.pass

#Create Zip file for Local update - no hsp
IPKS = recovery.ipk product-script.ipk software-update.ipk wpe.ipk brussels.ipk product.ipk lpm_updater.ipk
PACKAGENAMES = SoundTouchRecovery product-script software-update wpe brussels SoundTouch lpm_updater

EXCL_MANDATORY_PACKAGES_LST= product-script software-update hsp
EXCL_PACKAGES_LST_LOCAL=$(EXCL_MANDATORY_PACKAGES_LST)
EXCL_PACKAGES_LST_OTA=$(EXCL_MANDATORY_PACKAGES_LST)

.PHONY: generate-metadata
generate-metadata: cmake_build
	$(SOFTWARE_UPDATE_DIR)/make-metadata-json -d $(BOSE_WORKSPACE)/builds/$(cfg) -p professor,ginger-cheevers -k dev -l $(EXCL_PACKAGES_LST_LOCAL) -o $(EXCL_PACKAGES_LST_OTA)

.PHONY: packaage-no-hsp
package-no-hsp: packages-gz
	cd $(BOSE_WORKSPACE)/builds/$(cfg) && python2.7 $(SOFTWARE_UPDATE_DIR)/make-update-zip.py -n $(PACKAGENAMES) -i $(IPKS) -s $(BOSE_WORKSPACE)/builds/$(cfg) -d $(BOSE_WORKSPACE)/builds/$(cfg) -o product_update_no_hsp.zip -k $(privateKeyFilePath) -p $(privateKeyPasswordPath)

#Create one more Zip file for Bonjour / Local update with HSP
#- This is temporary, till DP2 boards are not available.
IPKS_HSP = recovery.ipk product-script.ipk software-update.ipk hsp.ipk wpe.ipk brussels.ipk product.ipk lpm_updater.ipk
PACKAGENAMES_HSP = SoundTouchRecovery product-script software-update hsp wpe brussels SoundTouch lpm_updater

.PHONY: package-with-hsp
package-with-hsp: packages-gz-with-hsp
	cd $(BOSE_WORKSPACE)/builds/$(cfg) && python2.7 $(SOFTWARE_UPDATE_DIR)/make-update-zip.py -n $(PACKAGENAMES_HSP) -i $(IPKS_HSP) -s $(BOSE_WORKSPACE)/builds/$(cfg) -d $(BOSE_WORKSPACE)/builds/$(cfg) -o product_update.zip -k $(privateKeyFilePath) -p $(privateKeyPasswordPath)

.PHONY: packages-gz
packages-gz: generate-metadata product-ipk wpe-ipk softwareupdate-ipk brussels-ipk hsp-ipk lpmupdater-ipk recovery-ipk product-script-ipk generate-metadata
	cd $(BOSE_WORKSPACE)/builds/$(cfg) && $(SOFTWARE_UPDATE_DIR)/make-packages-gz.sh Packages.gz $(IPKS)

.PHONY: packages-gz-with-hsp
packages-gz-with-hsp: generate-metadata brussels-ipk product-ipk wpe-ipk softwareupdate-ipk hsp-ipk lpmupdater-ipk recovery-ipk product-script-ipk generate-metadata
	cd $(BOSE_WORKSPACE)/builds/$(cfg) && $(SOFTWARE_UPDATE_DIR)/make-packages-gz.sh Packages.gz $(IPKS_HSP)

.PHONY: graph
graph: product-ipk
	graph-components --sdk=$(sdk) --exclude='CastleTools|TestUtils' Professor builds/$(cfg)/product-ipk-stage/component-info.gz -obuilds/$(cfg)/components

.PHONY: softwareupdate-ipk
softwareupdate-ipk: cmake_build
	./scripts/create-software-update-ipk

.PHONY: hsp-ipk
hsp-ipk: cmake_build
	./scripts/create-hsp-ipk $(cfg)

.PHONY: lpm-bos
lpm-bos:
ifneq ($(filter $(BUILD_TYPE), Release Continuous Nightly),)
	$(info BUILD_TYPE=$(BUILD_TYPE))
else
	$(error BUILD_TYPE must equal Release, Nightly or Continuous. Found $(BUILD_TYPE))
endif
	rm -f ./builds/$(cfg)/professor_package*.bos
	rm -f ./builds/$(cfg)/lpm_professor*.hex
	scripts/create-lpm-package ./builds/$(cfg)/ $(BUILD_TYPE) $(HW_VAR)

.PHONY: recovery-ipk
recovery-ipk: cmake_build
	./scripts/create-recovery-ipk -p professor

.PHONY: lpmupdater-ipk
lpmupdater-ipk: lpm-bos
	$(RIVIERALPMUPDATER_DIR)/create-ipk $(RIVIERALPMUPDATER_DIR)/lpm-updater-ipk-stage ./builds/$(cfg)/ ./builds/$(cfg)/ professor

.PHONY: brussels-ipk
brussels-ipk:
	./scripts/create-brussels-ipk

.PHONY: wpe-ipk
wpe-ipk:
	./scripts/create-wpe-ipk

.PHONY: product-script-ipk
product-script-ipk:
	./scripts/create-product-script-ipk

.PHONY: all-packages
all-packages: package-no-hsp package-with-hsp graph
	./scripts/create-product-tar -i $(IPKS_HSP)

.PHONY: clean
clean:
	rm -rf $(BOSE_WORKSPACE)/builds/$(cfg)/* builds/CastleTestUtils builds/__init__.py
	find . -name \*.pyc -delete

.PHONY: distclean
distclean:
	git clean -fdX
