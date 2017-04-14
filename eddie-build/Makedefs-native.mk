##############################################################################
# Native Linux
##############################################################################
$(info Building for native host...)

ifeq ($(LINUX_DISTRO), ubuntu)
GCC = gcc-4.7
GXX = g++-4.7
endif

ifeq ($(GLIBC_2_15_NEEDED), true)
export NATIVE_TOOLPATH   = /opt/centos/glibc-2.15-toolchain/bin/
export TOOLPATH          = /opt/centos/glibc-2.15-toolchain/bin/
CROSS_TOOL_CC  ?= $(TOOLPATH)$(GCC)
CROSS_TOOL_CPP ?= $(TOOLPATH)$(GXX)
else
export NATIVE_TOOLPATH   = /usr/bin/
export TOOLPATH          = /usr/bin/
endif

CROSS_COMPILE            =
CROSS_TOOL               =
CROSS_TOOL_PATH          = /usr
CROSS_OS_PATH            = /usr
CROSS_OSINC_PATH         = /usr

export ARCH              = x86

export LIBRARIAN         = /usr/bin/ar cr

export PLAT              = host
export UI_BUILD_DIR      = build-x86-linux

# For scons
export TARGET            = $(HOST_TARGET_NAME)
export TARGET_NAME=$(TARGET)
export CPU=$(TARGET_NAME)
