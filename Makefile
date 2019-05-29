export BOSE_WORKSPACE := $(abspath $(CURDIR))
include Settings.mk

.PHONY: deploy
deploy: all-packages
	scripts/collect-deployables builds/Release builds/deploy ${disableGVA}

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

PRODUCTCONTROLLERCOMMON_DIR = $(shell components get ProductControllerCommon installed_location)
RIVIERALPMUPDATER_DIR = $(shell components get RivieraLpmUpdater installed_location)
SOFTWARE_UPDATE_DIR = $(shell components get SoftwareUpdate-qc8017_32 installed_location)
GVA_DIR = $(shell components get GoogleVoiceAssistant-qc8017_64 installed_location)
AVSSERVICE_DIR = $(shell components get AVSService-qc8017_32 installed_location)
PRODUCT_STARTUP_DIR = $(shell components get product-startup installed_location)
RIVIERASWUPRECOVERY_DIR  = $(shell components get RivieraSwUpRecovery-qc8017_32 installed_location)
RIVIERAMINIMALFS_DIR  = $(shell components get RivieraMinimalFS-qc8017_32 installed_location)

.PHONY: generated_sources
generated_sources: check_tools $(VERSION_FILES)
	./scripts/check-features
	maybe-update $(BUILDS_DIR)/BoseFeatures.h -- ./scripts/create-features-h features.json $(BUILDS_DIR)/BoseFeatures.h
	$(MAKE) -C ProductController $@
	$(MAKE) -C $(PRODUCTCONTROLLERCOMMON_DIR) $@
	cp -av $(GVA_DIR)/tools/auth_util.py builds/$(cfg)
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

.PHONY: product-ipk
product-ipk: cmake_build
	./scripts/create-product-ipk

# The default build will always be signed using development keys
privateKeyFilePath = $(BOSE_WORKSPACE)/keys/development/privateKey/dev.p12
privateKeyPasswordPath = $(BOSE_WORKSPACE)/keys/development/privateKey/dev_p12.pass

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
	$(SOFTWARE_UPDATE_DIR)/make-metadata-json -d $(BOSE_WORKSPACE)/builds/$(cfg) -p eddie,eddieclub -k dev -l $(EXCL_PACKAGES_LST_LOCAL) -o $(EXCL_PACKAGES_LST_OTA)

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
packages-gz-no-gva: generate-metadata $(TARGETS_NOGVA)
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
	$(info BUILD_TYPE=$(BUILD_TYPE))
ifeq ($(filter $(BUILD_TYPE), Release Continuous Nightly),)
	$(error BUILD_TYPE must equal Release, Nightly or Continuous)
endif
	rm -f ./builds/$(cfg)/$(product)_package*.bos
	rm -f ./builds/$(cfg)/lpm_$(product)*.hex
	scripts/create-lpm-package ./builds/$(cfg)/ $(BUILD_TYPE)

.PHONY: recovery-ipk
recovery-ipk: cmake_build
	${RIVIERASWUPRECOVERY_DIR}/create-ipk -p $(product)

.PHONY: lpm_updater-ipk
lpm_updater-ipk: lpm-bos
	$(RIVIERALPMUPDATER_DIR)/create-ipk $(RIVIERALPMUPDATER_DIR)/lpm-updater-ipk-stage ./builds/$(cfg)/ ./builds/$(cfg)/ $(product)

.PHONY: monaco-ipk
monaco-ipk:
	./scripts/create-monaco-ipk

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
	/bin/bash ${RIVIERAMINIMALFS_DIR}/create-ipk

.PHONY: all-packages
all-packages: package-no-hsp package-with-hsp graph
	./scripts/create-product-tar -i $(IPKS_HSP)

ifeq (true,$(disableGVA))
all-packages: package-no-gva
endif

.PHONY: clean
clean:
	rm -rf $(BOSE_WORKSPACE)/builds/$(cfg) builds/CastleTestUtils builds/__init__.py builds/RivieraLpmService
	find . -name \*.pyc -delete

.PHONY: distclean
distclean:
	git clean -fdX
