WORKSPACE := $(abspath $(CURDIR)/..)
include $(WORKSPACE)/Settings.mk

CMAKE_FLAGS := -DCFG=$(cfg) -DSDK=$(sdk)
# BUILDS_DIR here becomes CMAKE_BINARY_DIR in CMake.

PROTOC := ../components/SoundTouch-SDK-qc8017_32/prebuilt/Release/native/bin/protoc

# TODO make any "install-SoundTouch-SDK" mods for qc and run it here (if we even need to)
PROTO_DIRS = \
 source/ProtoBufs

PROTO_CC_DEST = $(BUILDS_DIR)/proto

define build-proto
for dir in $(PROTO_DIRS); do make -C $$dir PROTO_CC_DEST=$(PROTO_CC_DEST) $1; done
endef

.PHONY: all
all: check_tools build_proto_sources cmake_build

.PHONY: clean_proto_sources
clean_proto_sources:
	$(call build-proto, clean)

.PHONY: check_tools
check_tools:
	@echo check tools. . .
	components install

.PHONY: cmake_build
cmake_build:
	@echo "Building Product Controller Code. . ."
	@echo BOSELIBS_DIR       : $(BOSELIBS_DIR)
	@echo SOUNDTOUCH_SDK_DIR : $(SOUNDTOUCH_SDK_DIR)
	@echo cmake_build
	@echo making directory $(BUILDS_DIR)
	mkdir -p $(BUILDS_DIR)
	rm -fv $(BUILDS_DIR)/CMakeCache.txt
	@echo BUILD_DIR : $(BUILDS_DIR) FLAGS : $(CMAKE_FLAGS) CURDIR : $(CURDIR)
	cd $(BUILDS_DIR) && cmake $(CMAKE_FLAGS) $(CURDIR)
	$(MAKE) -C $(BUILDS_DIR) -j $(jobs) install

.PHONY: build_proto_sources
build_proto_sources: $(SOUNDTOUCH_SDK_SENTINEL) $(HSP_DIR) $(QC_TOOLCHAIN_DIR)
	@echo "Building Product Controller Protocol Buffers. . ."
	$(call build-proto, all)

.PHONY: clean
clean:
	rm -rf $(BUILDS_DIR)
