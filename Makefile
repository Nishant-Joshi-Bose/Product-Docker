WORKSPACE := $(abspath $(CURDIR))
include Settings.mk

.PHONY: default
default: eddie-ipk

.PHONY: version-files
version-files: | $(BUILDS_DIR)
	gen-version-files version.txt $(BUILDS_DIR)

$(BUILDS_DIR):
	mkdir -p $@

.PHONY: ProductController
ProductController: install-components version-files
	$(MAKE) -C $@

.PHONY: install-components
install-components:
ifndef DONT_UPDATE_CASTLETOOLS
	castletools-update
endif
	components install

.PHONY: LpmService
LpmService: install-components
	$(MAKE) -C components/RivieraLpmService

.PHONY: eddie-ipk
eddie-ipk: ProductController LpmService
	./scripts/create-product-ipk

.PHONY: eddie-package
eddie-package: eddie-ipk
	./scripts/create-eddie-package

.PHONY: clean
clean:
	rm -rf $(BUILDS_DIR)

.PHONY: distclean
distclean:
	git clean -fdX
