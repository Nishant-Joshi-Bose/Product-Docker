WORKSPACE := $(abspath $(CURDIR))
include Settings.mk

.PHONY: default
default: product-ipk

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

.PHONY: product-ipk
product-ipk: ProductController LpmService
	rm -fv builds/$(cfg)/*.ipk
	./scripts/create-product-ipk

.PHONY: eddie-package
eddie-package: product-ipk
	./scripts/create-eddie-package

.PHONY: clean
clean:
	rm -rf $(BUILDS_DIR)

.PHONY: distclean
distclean:
	git clean -fdX
