export BOSE_WORKSPACE := $(abspath $(CURDIR))
include Settings.mk

ifneq ($(filter $(HW_VAR), DP2 Alpha),$(HW_VAR))
	$(error HW_VAR must equal DP2 or Alpha. Found $(HW_VAR))
endif

.PHONY: deploy
deploy: all-packages
	scripts/collect-deployables . builds/Release builds/deploy/$(HW_VAR) ${disableGVA}

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
GVA_DIR = $(shell components get GoogleVoiceAssistant-qc8017_64 installed_location)
AVSSERVICE_DIR = $(shell components get AVSService-qc8017_32 installed_location)
PRODUCT_STARTUP_DIR = $(shell components get product-startup installed_location)
RIVIERASWUPRECOVERY_DIR  = $(shell components get RivieraSwUpRecovery-qc8017_32 installed_location)
RIVIERAMINIMALFS_DIR  = $(shell components get RivieraMinimalFS-qc8017_32 installed_location)

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

.PHONY: product-ipk
product-ipk: cmake_build
	./scripts/create-product-ipk

# The default build will always be signed using development keys
privateKeyFilePath = $(BOSE_WORKSPACE)/keys/development/privateKey/dev.p12
privateKeyPasswordPath = $(BOSE_WORKSPACE)/keys/development/privateKey/dev_p12.pass

#Create Zip file for Local update - no hsp
IPKS_FILE=$(BOSE_WORKSPACE)/ipks.txt
PACKAGENAMES_FILE=$(BOSE_WORKSPACE)/package_names.txt

IPKS_HSP := $(shell cat < $(IPKS_FILE))
IPKS := $(filter-out hsp.ipk,$(IPKS_HSP))
IPKS_NOGVA := $(filter-out gva.ipk,$(IPKS_HSP))
PACKAGENAMES_HSP := $(shell cat < $(PACKAGENAMES_FILE))
PACKAGENAMES := $(filter-out hsp,$(PACKAGENAMES_HSP))
PACKAGENAMES_NOGVA := $(filter-out gva,$(PACKAGENAMES_HSP))
TARGETS_HSP := $(subst .ipk,-ipk,$(IPKS_HSP))
TARGETS := $(subst .ipk,-ipk,$(IPKS))
TARGETS_NOGVA := $(subst .ipk,-ipk,$(IPKS_NOGVA))

EXCL_MANDATORY_PACKAGES_LST= product-script software-update hsp
EXCL_PACKAGES_LST_LOCAL=$(EXCL_MANDATORY_PACKAGES_LST)
EXCL_PACKAGES_LST_OTA=$(EXCL_MANDATORY_PACKAGES_LST)

print_variables:
	echo $(IPKS_HSP)
	echo $(PACKAGENAMES_HSP)
	echo $(TARGETS_HSP)

# Add exclude packages list in metadata.json
.PHONY: generate-metadata
generate-metadata: cmake_build
	$(SOFTWARE_UPDATE_DIR)/make-metadata-json -d $(BOSE_WORKSPACE)/builds/$(cfg) -p professor,ginger-cheevers -k dev -l $(EXCL_PACKAGES_LST_LOCAL) -o $(EXCL_PACKAGES_LST_OTA)

.PHONY: package-no-hsp
package-no-hsp: packages-gz
	cd $(BOSE_WORKSPACE)/builds/$(cfg) && python2.7 $(SOFTWARE_UPDATE_DIR)/make-update-zip.py -n $(PACKAGENAMES) -i $(IPKS) -s $(BOSE_WORKSPACE)/builds/$(cfg) -d $(BOSE_WORKSPACE)/builds/$(cfg) -o product_update_no_hsp.zip -k $(privateKeyFilePath) -p $(privateKeyPasswordPath)

.PHONY: package-with-hsp
package-with-hsp: packages-gz-with-hsp
	cd $(BOSE_WORKSPACE)/builds/$(cfg) && python2.7 $(SOFTWARE_UPDATE_DIR)/make-update-zip.py -n $(PACKAGENAMES_HSP) -i $(IPKS_HSP) -s $(BOSE_WORKSPACE)/builds/$(cfg) -d $(BOSE_WORKSPACE)/builds/$(cfg) -o product_update.zip -k $(privateKeyFilePath) -p $(privateKeyPasswordPath)

.PHONY: packages-gz
packages-gz: generate-metadata $(TARGETS) 
	cd $(BOSE_WORKSPACE)/builds/$(cfg) && $(SOFTWARE_UPDATE_DIR)/make-packages-gz.sh Packages.gz $(IPKS)

.PHONY: packages-gz-with-hsp
packages-gz-with-hsp: generate-metadata $(TARGETS_HSP)
	cd $(BOSE_WORKSPACE)/builds/$(cfg) && $(SOFTWARE_UPDATE_DIR)/make-packages-gz.sh Packages.gz $(IPKS_HSP)

.PHONY: package-no-gva
package-no-gva: packages-gz-no-gva
	cd $(BOSE_WORKSPACE)/builds/$(cfg) && python2.7 $(SOFTWARE_UPDATE_DIR)/make-update-zip.py -n $(PACKAGENAMES_NOGVA) -i $(IPKS_NOGVA) -s $(BOSE_WORKSPACE)/builds/$(cfg) -d $(BOSE_WORKSPACE)/builds/$(cfg) -o product_update_nogva.zip -k $(privateKeyFilePath) -p $(privateKeyPasswordPath)

.PHONY: packages-gz-no-gva
packages-gz-no-gva: generate-metadata $(TARGES_NOGVA)
	cd $(BOSE_WORKSPACE)/builds/$(cfg) && $(SOFTWARE_UPDATE_DIR)/make-packages-gz.sh Packages.gz $(IPKS_NOGVA)

.PHONY: graph
graph: product-ipk
	graph-components --sdk=qc8017_32 --sdk=qc8017_64 --exclude='CastleTools|TestUtils' $(Product) builds/$(cfg)/product-ipk-stage/component-info.gz -obuilds/$(cfg)/components

.PHONY: software-update-ipk
software-update-ipk: cmake_build
	${SOFTWARE_UPDATE_DIR}/create-ipk

.PHONY: hsp-ipk
hsp-ipk: cmake_build
	./scripts/create-hsp-ipk $(cfg)

.PHONY: gva-ipk
gva-ipk: cmake_build
	${GVA_DIR}/create-ipk

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
	${RIVIERASWUPRECOVERY_DIR}/create-ipk -p professor

.PHONY: lpm-updater-ipk
lpm-updater-ipk: lpm-bos
	$(RIVIERALPMUPDATER_DIR)/create-ipk $(RIVIERALPMUPDATER_DIR)/lpm-updater-ipk-stage ./builds/$(cfg)/ ./builds/$(cfg)/ professor

.PHONY: brussels-ipk
brussels-ipk:
	./scripts/create-brussels-ipk

.PHONY: wpe-ipk
wpe-ipk:
	./scripts/create-wpe-ipk

.PHONY: product-script-ipk
product-script-ipk:
	${PRODUCT_STARTUP_DIR}/create-ipk

.PHONY: avs-ipk
avs-ipk:
	${AVSSERVICE_DIR}/create-ipk

.PHONY: minimalfs-ipk
minimalfs-ipk:
	/bin/bash ${RIVIERAMINIMALFS_DIR}/create-ipk -p ${product}

.PHONY: all-packages
ifeq (true,$(disableGVA))
all-packages: package-no-hsp package-with-hsp package-no-gva graph
else
all-packages: package-no-hsp package-with-hsp graph
endif
	./scripts/create-product-tar -i $(IPKS_HSP)

.PHONY: clean
clean:
	rm -rf $(BOSE_WORKSPACE)/builds/$(cfg)/* builds/CastleTestUtils builds/__init__.py
	find . -name \*.pyc -delete

.PHONY: distclean
distclean:
	git clean -fdX
