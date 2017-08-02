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

.PHONY: eddie-ipk
eddie-ipk: ProductController
	./scripts/create-eddie-ipk

.PHONY: clean
clean:
	rm -rf $(BUILDS_DIR)

.PHONY: distclean
distclean:
	git clean -fdX
