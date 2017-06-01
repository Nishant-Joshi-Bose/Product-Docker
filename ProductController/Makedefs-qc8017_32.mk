##############################################################################
# QualComm 8017 32-bit
##############################################################################
$(info Building for QC8017 32bit...)

export HOST_ARCH=$(shell uname -m)
SDK_VERSION              = $(shell cat ${TOPDIR}/../config/Toolchain-Version.txt)
$(info SDK_VERSION is ${SDK_VERSION})
CROSS_COMPILE            = arm-oemllib32-linux-
CROSS_TOOL               = arm-oemllib32-linux/$(CROSS_COMPILE)
export CROSS_TOOL_PATH		 = /scratch/${SDK_VERSION}/sysroots/${HOST_ARCH}-oesdk-linux/usr
CROSS_OS_PATH		 = /scratch/${SDK_VERSION}/sysroots/aarch64-oe-linux
CROSS_OSINC_PATH         = $(CROSS_OS_PATH)/usr/include

export CROSS_SYSROOT     = $(CROSS_OS_PATH)
# this is annoying
export TOOLPATH          = $(CROSS_TOOL_PATH)/bin/arm-oemllib32-linux/

export ARCH              = arm
ifeq ($(LINUX_DISTRO), centos)
export NATIVE_TOOLPATH   = /opt/centos/glibc-2.15-toolchain/bin/
else
export NATIVE_TOOLPATH   = /usr/bin/
endif
export LIBRARIAN         = $(CROSS_TOOL_PATH)/$(CROSS_TOOL)ar cr
PATH                    := $(PATH):$(TOOLPATH)
export TARGET_USR_PATH   = $(CROSS_OSINC_PATH)/usr/include

export PLAT              = target

export TARGET            = arm-oemllib32-linux


export TARGET_NAME=$(TARGET)
export CPU=$(TARGET_NAME)

CROSS_BIN_PATH = $(CROSS_TOOL_PATH)/bin
CROSS_LIB_PATH = "$(CROSS_OS_PATH)/usr/lib;$(CROSS_OS_PATH)/lib"
CROSS_INC_PATH = $(CROSS_OSINC_PATH)

CROSS_TOOL_CC  = $(CROSS_BIN_PATH)/$(CROSS_TOOL)$(GCC)
CROSS_TOOL_CPP = $(CROSS_BIN_PATH)/$(CROSS_TOOL)$(GXX)
CROSS_TOOL_STRIP = $(CROSS_BIN_PATH)/$(CROSS_TOOL)strip
CROSS_TOOL_CFLAGS = --sysroot ${CROSS_SYSROOT}

