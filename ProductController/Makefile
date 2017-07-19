WORKSPACE := $(abspath $(CURDIR)/..)
$(info WORKSPACE is $(WORKSPACE))

sdk = qc8017_32
ifeq ($(sdk),native)
  cfg = Debug
else
  cfg = Release
endif
jobs = 1

CMAKE_FLAGS := -DCFG=$(cfg) -DSDK=$(sdk)

BUILDS_DIR := $(WORKSPACE)/builds/$(cfg)/$(sdk)

.PHONY: all
all: check_tools package

.PHONY: check_tools
check_tools:
	components install

.PHONY: package
package: cmake_build
	$(MAKE) -C $(BUILDS_DIR) package

.PHONY: cmake_build
cmake_build:
	mkdir -p $(BUILDS_DIR)
	cd $(BUILDS_DIR) && cmake $(CMAKE_FLAGS) $(CURDIR)
	$(MAKE) -C $(BUILDS_DIR) -j $(jobs) install

.PHONY: clean
clean:
	rm -rf $(BUILDS_DIR)
