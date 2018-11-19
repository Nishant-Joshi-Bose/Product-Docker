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
ifndef DONT_INSTALL_COMPONENTS
	components install
endif

CMAKE_USE_CCACHE := $(USE_CCACHE)

RIVIERALPM_DIR = $(shell components get RivieraLPM installed_location)
RIVIERA_LPM_TOOLS_DIR = $(shell components get RivieraLpmTools installed_location)
PRODUCTCONTROLLERCOMMON_DIR = $(shell components get ProductControllerCommon installed_location)
RIVIERALPMUPDATER_DIR = $(shell components get RivieraLpmUpdater installed_location)
SOFTWARE_UPDATE_DIR = $(shell components get SoftwareUpdate-qc8017_32 installed_location)
TESTUTILS_DIR = $(shell components get TestUtils installed_location)
PRODUCTCONTROLLERCOMMONPROTO_DIR = $(shell components get ProductControllerCommonProto installed_location)

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

USERKEYCONFIG=$(PWD)/opt-bose-fs/etc/UserKeyConfig.json
KEYCONFIG=$(PWD)/opt-bose-fs/etc/KeyConfiguration.json
LPM_KEYS=$(RIVIERALPM_DIR)/include/RivieraLPM_KeyValues.h
INTENT_DEFS=$(PWD)/ProductController/IntentHandler/Intents.h
KEYCONFIG_INCS=$(PRODUCTCONTROLLERCOMMON_DIR)/IntentHandler

.PHONY: keyconfig
keyconfig: check_tools
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
		--net $(LPM_KEYS) \
		--outputcfg $(KEYCONFIG) \
		--incdirs $(KEYCONFIG_INCS)

USERBLASTCONFIG=$(PWD)/Config/UserKeyConfig.json
BLASTCONFIG=$(PWD)/opt-bose-fs/etc/BlastConfiguration.json
LPM_KEYS=$(RIVIERALPM_DIR)/include/RivieraLPM_KeyValues.h

.PHONY: blastconfig
blastconfig: check_tools
	cd tools/key_config_generator && \
	./generate_blast_config \
		$(BUILDS_DIR) \
		--inputcfg $(USERBLASTCONFIG) \
		--keyfile $(LPM_KEYS) \
		--outputcfg $(BLASTCONFIG)

.PHONY: cmake_build
cmake_build: generated_sources | $(BUILDS_DIR) astyle
	rm -rf $(BUILDS_DIR)/CMakeCache.txt $(BUILDS_DIR)/CMakeFiles
	cd $(BUILDS_DIR) && cmake -DCFG=$(cfg) -DSDK=$(sdk) $(CURDIR) -DUSE_CCACHE=$(CMAKE_USE_CCACHE)
	$(MAKE) -C $(BUILDS_DIR) -j $(jobs) install

.PHONY: minimal-product-tar
minimal-product-tar: cmake_build
	./scripts/create-minimal-product-tar

.PHONY: product-ipk
product-ipk: cmake_build
	./scripts/create-product-ipk

# The default build will always be signed using development keys
privateKeyFilePath = $(BOSE_WORKSPACE)/keys/development/privateKey/dev.p12
privateKeyPasswordPath = $(BOSE_WORKSPACE)/keys/development/privateKey/dev_p12.pass

#Create Zip file for Local update - no hsp
IPKS = recovery.ipk product-script.ipk software-update.ipk wpe.ipk monaco.ipk product.ipk lpm_updater.ipk
PACKAGENAMES = SoundTouchRecovery product-script software-update wpe monaco SoundTouch lpm_updater

.PHONY: package-no-hsp
package-no-hsp: packages-gz
	cd $(BOSE_WORKSPACE)/builds/$(cfg) && python2.7 $(SOFTWARE_UPDATE_DIR)/make-update-zip.py -n $(PACKAGENAMES) -i $(IPKS) -s $(BOSE_WORKSPACE)/builds/$(cfg) -d $(BOSE_WORKSPACE)/builds/$(cfg) -o product_update_no_hsp.zip -k $(privateKeyFilePath) -p $(privateKeyPasswordPath)

#Create one more Zip file for Bonjour / Local update with HSP 
#- This is temporary, till DP2 boards are not available.
IPKS_HSP = recovery.ipk product-script.ipk software-update.ipk hsp.ipk wpe.ipk monaco.ipk product.ipk lpm_updater.ipk
PACKAGENAMES_HSP = SoundTouchRecovery product-script software-update hsp wpe monaco SoundTouch lpm_updater

.PHONY: package-with-hsp
package-with-hsp: packages-gz-with-hsp
	cd $(BOSE_WORKSPACE)/builds/$(cfg) && python2.7 $(SOFTWARE_UPDATE_DIR)/make-update-zip.py -n $(PACKAGENAMES_HSP) -i $(IPKS_HSP) -s $(BOSE_WORKSPACE)/builds/$(cfg) -d $(BOSE_WORKSPACE)/builds/$(cfg) -o product_update.zip -k $(privateKeyFilePath) -p $(privateKeyPasswordPath)

.PHONY: packages-gz
packages-gz: product-ipk wpe-ipk softwareupdate-ipk monaco-ipk hsp-ipk lpmupdater-ipk recovery-ipk product-script-ipk
	cd $(BOSE_WORKSPACE)/builds/$(cfg) && $(SOFTWARE_UPDATE_DIR)/make-packages-gz.sh Packages.gz $(IPKS)

.PHONY: packages-gz-with-hsp
packages-gz-with-hsp: monaco-ipk product-ipk wpe-ipk softwareupdate-ipk hsp-ipk lpmupdater-ipk recovery-ipk product-script-ipk
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
recovery-ipk: cmake_build minimal-product-tar
	./scripts/create-recovery-ipk

.PHONY: lpmupdater-ipk
lpmupdater-ipk: lpm-bos
	$(RIVIERALPMUPDATER_DIR)/create-ipk $(RIVIERALPMUPDATER_DIR)/lpm-updater-ipk-stage ./builds/$(cfg)/ ./builds/$(cfg)/ professor

.PHONY: monaco-ipk
monaco-ipk:
	./scripts/create-monaco-ipk

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
