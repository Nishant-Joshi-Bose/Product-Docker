subdirs = ProductController

.PHONY: $(subdirs)

.PHONY: default
default: $(subdirs)

.PHONY: ProductController
ProductController: install-components
	$(MAKE) -C $@

.PHONY: install-components
install-components:
	components install
