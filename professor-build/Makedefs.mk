export HOST_SDK=native
export CFG_DEBUG=Debug
export HOST_TARGET_NAME=x86-unknown-linux

# Defaults, overriden by command-line.
export sdk ?= $(HOST_SDK)
export cfg ?= $(CFG_DEBUG)
export jobs ?= 1

GLIBC_2_15_NEEDED:=$(shell [ `ldd --version | \grep '^ldd' | sed 's/ldd (.*) \([0-9]\+\)\.\([0-9]\+\)/\2/'` -lt 15 ] && echo true )
export GLIBC_2_15_NEEDED

# This should return centos or ubuntu
LINUX_DISTRO:=$(shell lsb_release -a 2>&1 | \grep -i Distributor | sed "s/.*:\s//" | tr A-Z a-z)
export LINUX_DISTRO

# Set default gcc/g++ binaries
GCC = gcc
GXX = g++

include Makedefs-${sdk}.mk

CROSS_BIN_PATH ?= $(CROSS_TOOL_PATH)/bin
CROSS_LIB_PATH ?= $(CROSS_OS_PATH)/lib
CROSS_INC_PATH ?= $(CROSS_OSINC_PATH)/include

CROSS_TOOL_CC  ?= $(CROSS_BIN_PATH)/$(CROSS_TOOL)$(GCC)
CROSS_TOOL_CPP ?= $(CROSS_BIN_PATH)/$(CROSS_TOOL)$(GXX)
CROSS_TOOL_STRIP ?= $(CROSS_BIN_PATH)/$(CROSS_TOOL)strip

export CC=$(CROSS_TOOL_CC)
export CXX=$(CROSS_TOOL_CPP)

export LIB_OPT_PREFIX=
export LIB_OPT_PREFIX=
export OBJ_EXT=o
export LIB_PREFIX=lib
export LIB_EXT=a

# Set CCACHE_DIR unless it's already defined and display a warning
# if the max cache size is still set to the default of 1.0 GB.
ifndef CCACHE_DIR
    export CCACHE_DIR=/scratch/ccache
endif

CP    = cp
RM    = rm -f
MV    = mv
RMDIR = rmdir
MKDIR = mkdir
NULL  = /dev/null

MD5SUM    = md5sum
MKISOFS   = mkisofs
TAR       = tar

##############################################################################
# Output paths
##############################################################################

# All intermediate and final output files go under this tree.
TOPBUILDDIR=$(TOPDIR)/build/$(cfg)/$(sdk)

# Final resting place for libraries.
LIBDIR=$(TOPBUILDDIR)/lib

# Final resting place for product binaries (executables).
BINDIR=$(TOPBUILDDIR)/bin

# Where to write generated *.proto, *.pb.h and *.pb.cc files.
PROTO_CC_DEST := $(TOPDIR)/builds/$(cfg)/$(sdk)/proto

# System-wide build defines. Individual makefiles can add additional defines by
# defining EXTRA_DEFS.
COMMON_DEFS=-D_REENTRANT

DEFINES=$(COMMON_DEFS) $(EXTRA_DEFS)
