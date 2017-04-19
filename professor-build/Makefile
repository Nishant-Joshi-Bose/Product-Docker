#########################################################################
#
# Professor top level makefile.
#
# Copyright 2016 Bose Corporation, Framingham, MA
#
#########################################################################

MKFILE_DIR := $(shell dirname $(abspath $(lastword $(MAKEFILE_LIST))))
TOPDIR := ${MKFILE_DIR}

export # all make variables automatically exported to children's environment

cfg ?= Release
sdk ?= qc8017_32

include Makedefs.mk

.PHONY: default
default: all

ifdef TERM
  RED:=$(shell tput setaf 1)
  GREEN:=$(shell tput setaf 2)
  YELLOW:=$(shell tput setaf 3)
  BLUE:=$(shell tput setaf 4)
  MAGENTA:=$(shell tput setaf 5)
  CYAN:=$(shell tput setaf 6)
  WHITE:=$(shell tput setaf 7)
  RESET_COLOR:=$(shell tput sgr0)
endif

ifeq (,$(filter '' 'true' 'false','$(LINKS)'))
    $(error $(RED)If you use the LINKS option it must be set to "true" or "false". See 'make help' for more info.$(RESET_COLOR))
endif
ifeq (,$(filter '' 'true' 'false','$(PRODUCTION)'))
    $(error $(RED)If you use the PRODUCTION option it must be set to "true" or "false". See 'make help' for more info.$(RESET_COLOR))
endif
ifeq (,$(filter '' 'true' 'false','$(BUILD_SHELBY)'))
    $(error $(RED)If you use the BUILD_SHELBY option it must be set to "true" or "false". See 'make help' for more info.$(RESET_COLOR))
endif

#ifneq ($(MAKECMDGOALS),clean)
#    ifneq (true,$(LINKS))
#        ifeq (,$(QC_HSP_DIST)$(SOUNDTOUCH_SDK_DIST))
#            # If we are in the Shelby repo assume the user wants to build against the locally built tarballs.
#            ifeq ("https://svn.bose.com/hepd/Shelby","$(shell svn info .. 2>/dev/null | awk '/^Repository Root:/ {print $$NF}')")
#                QC_HSP_DIST=
#                SOUNDTOUCH_SDK_DIST=../../builds
#                $(info $(MAGENTA)You are building Professor in the context of the Shelby repo.$(RESET_COLOR))
#                $(info $(MAGENTA)Assuming you wish to use your local STSDK and HSP:$(RESET_COLOR))
#                $(info $(MAGENTA)    SOUNDTOUCH_SDK_DIST: $(SOUNDTOUCH_SDK_DIST)/SoundTouch-SDK-qc8017_32.tar$(RESET_COLOR))
##                $(info $(MAGENTA)    TI_HSP_DIST: $(TI_HSP_DIST)/ti-sm2-hsp.tar$(RESET_COLOR))
##                $(info $(MAGENTA)                 $(TI_HSP_DIST)/ti-sm1-hsp.tar$(RESET_COLOR))
##                $(info $(MAGENTA)                 $(TI_HSP_DIST)/ti-sm1-noap-hsp.tar$(RESET_COLOR))
#                $(info $(MAGENTA)If this is not what you wanted, do a 'make clean' and use one of the SOUNDTOUCH, QC-HSP, or LINKS options.$(RESET_COLOR))
#             endif
#        endif
#    endif
#endif

#
# QC-HSP
#
ifdef QC_HSP_DIST
  # The user doesn't want to use the QC-HSP tarball pointed to from
  # QC-HSP-link.txt, they specified it on the command line with the
  # QC_HSP_DIST variable.
  QC_HSP_TAR := $(QC_HSP_DIST)
else
  # The QC-HSP tarballs are identified in QC-HSP-link.txt
  QC_HSP_TAR := $(shell cat ../config/HSP-Version.txt)

  ifndef QC_HSP_TAR
    $(error Oops, something is wrong with HSP-Version.txt)
  endif
endif

# double basename because it's .tar.gz
HSP_DIR = $(basename $(basename $(notdir $(QC_HSP_TAR))))

# Unpack the HSP
$(HSP_DIR): $(QC_HSP_TAR)
	@echo -e "$(CYAN)Extracting QC HSP tarball$(RESET_COLOR)"
#	@echo "HSP_DIR = $(HSP_DIR)"
	@rm -rf $(HSP_DIR)
	@tar -xmf $(QC_HSP_TAR)
	@test -d $@ # assert the file exists
	@touch $@

#ifneq ($(MAKECMDGOALS),clean)
#  -include sm2-HSP/SampleMakefile/Makedefs.mk
#endif

#
# Toolchain install
#
ifndef QC_TOOLCHAIN
  QC_TOOLCHAIN = $(shell cat ../config/Toolchain-Version.txt)
  ifndef QC_TOOLCHAIN
    $(error Oops, something is wrong with Toolchain-Version.txt)
  endif
endif

QC_TOOLCHAIN_DIR=/scratch/$(QC_TOOLCHAIN)

# Install the toolchain (Note: this currently only deals with 32-bit host)
# TBD make this work for 32/64 bit host when the HSP actually contains one
$(QC_TOOLCHAIN_DIR): $(HSP_DIR)
	@echo -e "$(CYAN)Installing QC toolchain $(RESET_COLOR)"
	@rm -rf $(QC_TOOLCHAIN_DIR)
	@yes | $(HSP_DIR)/sdk/install-sdk.sh -d $@
	@test -d $@
	@touch $@

#
# SoundTouch-SDK
#
ifdef SOUNDTOUCH_SDK_DIST
  SOUNDTOUCH_SDK_TAR := $(SOUNDTOUCH_SDK_DIST)
else
#  # If they specify building from shelby sources, do that
#  ifeq(${BUILD_SHELBY},true)
#  else
    # The SoundStouch-SDK tarball is identified in SoundTouch-link.txt
    SOUNDTOUCH_SDK_TAR := $(shell cat ../config/SoundTouchSDK-Version-${sdk}.txt)
    ifndef SOUNDTOUCH_SDK_TAR
      $(error Oops, something is wrong with SoundTouchSDK-Version-${sdk}.txt)
    endif
#  endif
endif

SOUNDTOUCH_SDK_SENTINEL := builds/SoundTouch-SDK-${sdk}/install-SoundTouch-SDK

$(SOUNDTOUCH_SDK_SENTINEL): $(SOUNDTOUCH_SDK_TAR)
	@echo -e "$(CYAN)Extracting SoundTouch-SDK$(RESET_COLOR)"
	rm -rf builds/SoundTouch-SDK-${sdk}
	tar -xmf $(SOUNDTOUCH_SDK_TAR)
	-mkdir builds
	mv SoundTouch-SDK builds/SoundTouch-SDK-${sdk}
	test -f $@ # assert the file exists
	touch $@

# TODO make any "install-SoundTouch-SDK" mods for qc and run it here (if we even need to)


.PHONY: all
all:: all_packages

.PHONY: rebuild
rebuild: clean all

.PHONY: help
help:
	@echo
	@echo "Make targets :"
	@echo "  all (default)         - compile and package everything"
	@echo "  clean                 - remove all the generated files"
	@echo
	@echo "Make Variables :"
#	@echo "  PRODUCTION=[true|false]                 - create a production file system (streaming)"
	@echo "  SOUNDTOUCH_SDK_DIST=[tarballfile]       - points to the SoundTouch-SDK's tarball"
	@echo "  QC_HSP_DIST=[tarballfile]               - points to the QC-HSP tarball"
#	@echo "  LINKS=[true|false]                      - use the tarballs pointed to in SoundTouch-link.txt and QC-HSP-link.txt"
	@echo
#	@echo "For builds performed from the Shelby repo the default directory for the SoundTouch-SDK and QC-HSP tarballs are ../../builds."
#	@echo "For builds performed independently form the Shelby repo the default locations are defined by SoundTouch-link.txt and QC-HSP-link.txt."
#	@echo "If you wish to override one or both, you can use the LINKS, SOUNDTOUCH_SDK_DIST and QC_HSP_DIST variables on the command line."


# CMAKE stuff
CMAKE = cmake
BUILDS_ROOT_DIR=$(MKFILE_DIR)/builds
BUILDS_DIR=$(BUILDS_ROOT_DIR)/$(cfg)/$(sdk)

.PHONY: cmake_build
cmake_build: $(SOUNDTOUCH_SDK_SENTINEL) #\
#$(TARGET_LIBS_DIR) $(TARGET_BINS_DIR)
	@echo -e "$(CYAN)Preparing the cmake build...$(RESET_COLOR)"
	-@echo "Building via CMAKE for $(sdk) with jobs=$(jobs)"
	mkdir -p $(BUILDS_DIR)
	cd $(BUILDS_DIR) && $(CMAKE) -DCFG=${cfg} -DSDK=${sdk} -DCORE_TARGET=$(TARGET_NAME) -DINC_PATH=$(CROSS_INC_PATH) -DLIB_PATH=$(CROSS_LIB_PATH) -DUSR_LIB_PATH=$(CROSS_LIB_PATH) -DCMAKE_C_COMPILER=$(CROSS_TOOL_CC) -DCMAKE_CXX_COMPILER=$(CROSS_TOOL_CPP) -DCMAKE_C_FLAGS="${CROSS_TOOL_CFLAGS}" -DCMAKE_CXX_FLAGS="${CROSS_TOOL_CFLAGS}" -DTOOLPATH=$(TOOLPATH) -DCROSS_COMPILE=$(CROSS_COMPILE) -DEXTRA_FLAGS='$(EXTRA_FLAGS)' -DLINUX_DISTRO=$(LINUX_DISTRO) ../../../.
	@echo -e "$(CYAN)Starting the Professor build...$(RESET_COLOR)"
	make -C $(BUILDS_DIR) -j $(jobs) install

STAGE_DIR = builds/$(cfg)/$(sdk)/stage
DIR_COPY=cp -RPfa
CP=/bin/cp -Pfa

# TODO make this build an ipk
.PHONY: package_professor
package_professor: cmake_build
#	@echo -e "$(CYAN)Building the filesystem$(RESET_COLOR)"
#	rm -rf $(STAGE_DIR_SM2)
#	mkdir -p $(STAGE_DIR_SM2)
#	$(SM2_HSP)/utils/install-hsp -Xir-sleep -Xamp-faults $(STAGE_DIR_SM2)
#	$(CP) "SoundTouch-SDK/AudioConfig/sm2/APConfig-rhino.xml" $(STAGE_DIR_SM2)/etc/opt/Bose
#	$(INSTALL_SOUNDTOUCH_SDK) -Sproduct-config=Shepherd.xml $(STAGE_DIR_SM2) $(SM2_CFG_DIR) rhino
#
## Copy Rhino binaries
#	$(CROSS_TOOL_STRIP) builds/Release/arm-linux/Rhino -o $(STAGE_DIR_SM2)/opt/Bose/Rhino
#
## Copy Rhino config files
#	$(CP) ./assets/rhino_*.cfg $(STAGE_DIR_SM2)/lib/firmware
#	./create-rhino-update-file sm2 $(SM2_HSP)/update/SCM_Update.xml $(SM2_IDX_DEST_DIR)
#	$(CP) $(SM2_IDX_DEST_DIR)/SCM_Update.xml $(STAGE_DIR_SM2)/etc/opt/Bose/SCM_Update.xml
#
## Create UBI image
#	$(SM2_HSP)/utils/create-fs-image $(STAGE_DIR_SM2) builds/$(SM2_FS)

.PHONY: all_packages
all_packages: package_professor


.PHONY: clean
clean:
	rm -rf builds output $(HSP_DIR) SoundTouch-SDK

.PHONY: cmake_clean
cmake_clean: clean
