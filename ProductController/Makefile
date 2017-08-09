WORKSPACE := $(abspath $(CURDIR)/..)
include $(WORKSPACE)/Settings.mk

CMAKE_FLAGS := -DCFG=$(cfg) -DSDK=$(sdk)
# BUILDS_DIR here becomes CMAKE_BINARY_DIR in CMake.

.PHONY: default
default: all

PROTO_DIRS = \
 Protobufs

PROTO_CC_DEST := $(BUILDS_DIR)/proto

define build-proto
for dir in $(PROTO_DIRS); do make -C $$dir PROTO_CC_DEST=$(PROTO_CC_DEST) $1; done
endef

.PHONY: build_protobufs
build_protobufs:
	@echo Building protocol buffer sources from proto files
	$(call build-proto, all)

.PHONY: clean_protobufs
clean_protobufs:
	$(call build-proto, clean)

.PHONY: all
all: check_tools cmake_build

.PHONY: check_tools
check_tools:
	components install

.PHONY: cmake_build
cmake_build: build_protobufs
	mkdir -p $(BUILDS_DIR)
	rm -fv $(BUILDS_DIR)/CMakeCache.txt
	cd $(BUILDS_DIR) && cmake $(CMAKE_FLAGS) $(CURDIR)
	$(MAKE) -C $(BUILDS_DIR) -j $(jobs) install

.PHONY: clean
clean:
	rm -rf $(BUILDS_DIR)
