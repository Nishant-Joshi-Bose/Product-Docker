WORKSPACE := $(abspath $(CURDIR)/..)
$(info WORKSPACE is $(WORKSPACE))

sdk = qc8017_32
ifeq ($(sdk),native)
  cfg = Debug
else
  cfg = Release
endif
jobs = 1

BUILDS_DIR := $(WORKSPACE)/builds/$(cfg)/$(sdk)

SOUNDTOUCH_SDK_DIR := $(shell components get SoundTouch-SDK-$(sdk) installed_location)
$(info SOUNDTOUCH_SDK_DIR is $(SOUNDTOUCH_SDK_DIR))

# -----------------------------------------------------------------------------

ifeq ($(sdk),native)
  TARGET := x86-unknown-linux
  CROSS_COMPILE :=
  CROSS_TOOL :=
  CROSS_TOOL_PATH := /usr
  CROSS_OS_PATH := /usr
  CROSS_SYSROOT :=
  CROSS_LIB_PATH :=
  CROSS_INC_PATH :=
  TOOLPATH := /opt/centos/glibc-2.15-toolchain/bin/
  CROSS_TOOL_CC := $(TOOLPATH)gcc
  CROSS_TOOL_CPP := $(TOOLPATH)g++
endif # native

ifeq ($(sdk),qc8017_32)
  RIVIERA_HSP_DIR := $(shell components get Riviera-HSP installed_location)
  $(info RIVIERA_HSP_DIR is $(RIVIERA_HSP_DIR))
  TARGET := arm-oemllib32-linux
  CROSS_COMPILE := arm-oemllib32-linux-
  CROSS_TOOL := arm-oemllib32-linux/$(CROSS_COMPILE)
  CROSS_TOOL_PATH := $(RIVIERA_HSP_DIR)/sdk/sysroots/i686-oesdk-linux/usr
  CROSS_OS_PATH := $(RIVIERA_HSP_DIR)/sdk/sysroots/aarch64-oe-linux
  CROSS_SYSROOT := --sysroot $(CROSS_OS_PATH)
  CROSS_LIB_PATH := $(CROSS_OS_PATH)/usr/lib;$(CROSS_OS_PATH)/lib
  CROSS_INC_PATH := $(CROSS_OS_PATH)/usr/include
  CROSS_TOOL_CC := $(CROSS_TOOL_PATH)/bin/$(CROSS_TOOL)gcc
  CROSS_TOOL_CPP := $(CROSS_TOOL_PATH)/bin/$(CROSS_TOOL)g++
endif # qc8017_32

# -----------------------------------------------------------------------------

.PHONY: all
all: check_tools package

.PHONY: check_tools
check_tools:
	components install

.PHONY: package
package: cmake_build
	$(MAKE) -C $(BUILDS_DIR) package

cmake_flags := \
-DCFG=$(cfg) \
-DSDK=$(sdk) \
-DCORE_TARGET=$(TARGET) \
-DINC_PATH='$(CROSS_INC_PATH)' \
-DLIB_PATH='$(CROSS_LIB_PATH)' \
-DUSR_LIB_PATH='$(CROSS_LIB_PATH)' \
-DCMAKE_C_COMPILER=$(CROSS_TOOL_CC) \
-DCMAKE_CXX_COMPILER=$(CROSS_TOOL_CPP) \
-DCMAKE_C_FLAGS='$(CROSS_SYSROOT)' \
-DCMAKE_CXX_FLAGS='$(CROSS_SYSROOT)' \
-DCROSS_COMPILE=$(CROSS_COMPILE) \
-DEXTRA_FLAGS='$(EXTRA_FLAGS)' \
-DSOUNDTOUCH_SDK_DIR=$(SOUNDTOUCH_SDK_DIR) \

.PHONY: cmake_build
cmake_build:
	mkdir -p $(BUILDS_DIR)
	cd $(BUILDS_DIR) && cmake $(cmake_flags) $(CURDIR)
	$(MAKE) -C $(BUILDS_DIR) -j $(jobs) install

.PHONY: clean
clean:
	rm -rf $(BUILDS_DIR)
