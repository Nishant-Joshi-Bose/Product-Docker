export BOSE_WORKSPACE := $(abspath $(CURDIR))
include Settings.mk

.PHONY: deploy
deploy: all-packages
	scripts/collect-deployables builds/Release builds/deploy

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

RIVIERA_LPM_TOOLS_DIR = $(shell components get RivieraLPM-Tools installed_location)
PRODUCTCONTROLLERCOMMON_DIR = $(shell components get ProductControllerCommon installed_location)
RIVIERALPMUPDATER_DIR = $(shell components get RivieraLpmUpdater installed_location)
SOFTWARE_UPDATE_DIR = $(shell components get SoftwareUpdate-qc8017_32 installed_location)
TESTUTILS_DIR = $(shell components get TestUtils installed_location)
RIVIERA_LPM_SERVICE_DIR = $(shell components get RivieraLpmService-qc8017_32 installed_location)
PRODUCTCONTROLLERCOMMONPROTO = $(shell components get ProductControllerCommonProto installed_location)

.PHONY: generated_sources
generated_sources: check_tools $(VERSION_FILES)
	$(MAKE) -C ProductController $@
	$(MAKE) -C $(PRODUCTCONTROLLERCOMMON_DIR) $@
	ln -nsf $(TESTUTILS_DIR) builds/CastleTestUtils
	ln -nsf $(RIVIERA_LPM_SERVICE_DIR) builds/RivieraLpmService
	touch builds/__init__.py

.PHONY: astyle
astyle:
ifndef DONT_RUN_ASTYLE
	run-astyle
endif

.PHONY: cmake_build
cmake_build: generated_sources | $(BUILDS_DIR) astyle
	rm -rf $(BUILDS_DIR)/CMakeCache.txt
	cd $(BUILDS_DIR) && $(CMAKE) -DCFG=$(cfg) -DSDK=$(sdk) $(CURDIR) -DUSE_CCACHE=$(CMAKE_USE_CCACHE)
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

IPKS = recovery.ipk product-script.ipk software-update.ipk wpe.ipk toledo.ipk product.ipk lpm_updater.ipk
PACKAGENAMES = SoundTouchRecovery product-script software-update wpe toledo SoundTouch lpm_updater

EXCL_MANDATORY_PACKAGES_LST= product-script software-update hsp
EXCL_PACKAGES_LST_LOCAL=$(EXCL_MANDATORY_PACKAGES_LST)
EXCL_PACKAGES_LST_OTA=$(EXCL_MANDATORY_PACKAGES_LST)

# Metadata proto file did not have "option (ignore_unexpected_markup) =
# true;" option enabled and hence for the device having this proto
# version can not use new tags from metadata.json file.
# So for current scenario we will use exclude list from
# "/opt/Bose/update/etc/SwUpExcludePackagesList" file and when we decide
# to use exclude list from metadata.json we can enable below options.

.PHONY: generate-metadata
generate-metadata:
	$(SOFTWARE_UPDATE_DIR)/make-metadata-json -d $(BOSE_WORKSPACE)/builds/$(cfg) -p $(product) -k dev #-l $(EXCL_PACKAGES_LST_LOCAL) -o $(EXCL_PACKAGES_LST_OTA)

.PHONY: package-no-hsp
package-no-hsp: packages-gz
	cd $(BOSE_WORKSPACE)/builds/$(cfg) && python2.7 $(SOFTWARE_UPDATE_DIR)/make-update-zip.py -n $(PACKAGENAMES) -i $(IPKS) -s $(BOSE_WORKSPACE)/builds/$(cfg) -d $(BOSE_WORKSPACE)/builds/$(cfg) -o product_update_no_hsp.zip -k $(privateKeyFilePath) -p $(privateKeyPasswordPath)

#Create one more Zip file for Bonjour / Local update with HSP
#- This is temporary, till DP2 boards are not available.
IPKS_HSP = recovery.ipk product-script.ipk software-update.ipk hsp.ipk wpe.ipk toledo.ipk product.ipk lpm_updater.ipk
PACKAGENAMES_HSP = SoundTouchRecovery product-script software-update hsp wpe toledo SoundTouch lpm_updater

.PHONY: package-with-hsp
package-with-hsp: packages-gz-with-hsp
	cd $(BOSE_WORKSPACE)/builds/$(cfg) && python2.7 $(SOFTWARE_UPDATE_DIR)/make-update-zip.py -n $(PACKAGENAMES_HSP) -i $(IPKS_HSP) -s $(BOSE_WORKSPACE)/builds/$(cfg) -d $(BOSE_WORKSPACE)/builds/$(cfg) -o product_update.zip -k $(privateKeyFilePath) -p $(privateKeyPasswordPath)

.PHONY: packages-gz
packages-gz: product-ipk wpe-ipk softwareupdate-ipk toledo-ipk hsp-ipk lpmupdater-ipk recovery-ipk product-script-ipk generate-metadata
	cd $(BOSE_WORKSPACE)/builds/$(cfg) && $(SOFTWARE_UPDATE_DIR)/make-packages-gz.sh Packages.gz $(IPKS)

.PHONY: packages-gz-with-hsp
packages-gz-with-hsp: toledo-ipk product-ipk wpe-ipk softwareupdate-ipk hsp-ipk lpmupdater-ipk recovery-ipk product-script-ipk generate-metadata
	cd $(BOSE_WORKSPACE)/builds/$(cfg) && $(SOFTWARE_UPDATE_DIR)/make-packages-gz.sh Packages.gz $(IPKS_HSP)

.PHONY: graph
graph: product-ipk
	graph-components --sdk=$(sdk) --exclude='CastleTools|TestUtils' $(Product) builds/$(cfg)/product-ipk-stage/component-info.gz -obuilds/$(cfg)/components

.PHONY: softwareupdate-ipk
softwareupdate-ipk: cmake_build
	./scripts/create-software-update-ipk

.PHONY: hsp-ipk
hsp-ipk: cmake_build
	./scripts/create-hsp-ipk $(cfg)

.PHONY: lpm-bos
lpm-bos:
	$(info BUILD_TYPE=$(BUILD_TYPE))
ifeq ($(filter $(BUILD_TYPE), Release Continuous Nightly),)
	$(error BUILD_TYPE must equal Release, Nightly or Continuous)
endif
	rm -f ./builds/$(cfg)/$(product)_package*.bos
	rm -f ./builds/$(cfg)/lpm_$(product)*.hex
	scripts/create-lpm-package ./builds/$(cfg)/ $(BUILD_TYPE)

.PHONY: recovery-ipk
recovery-ipk: cmake_build minimal-product-tar
	./scripts/create-recovery-ipk

.PHONY: lpmupdater-ipk
lpmupdater-ipk: lpm-bos
	$(RIVIERALPMUPDATER_DIR)/create-ipk $(RIVIERALPMUPDATER_DIR)/lpm-updater-ipk-stage ./builds/$(cfg)/ ./builds/$(cfg)/ $(product)

.PHONY: toledo-ipk
toledo-ipk:
	./scripts/create-toledo-ipk

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
	rm -rf $(BOSE_WORKSPACE)/builds/$(cfg) builds/CastleTestUtils builds/__init__.py builds/RivieraLpmService
	find . -name \*.pyc -delete

.PHONY: distclean
distclean:
	git clean -fdX
