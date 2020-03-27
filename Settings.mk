product = professor
PRODUCT = PROFESSOR
Product = Professor

ifndef BOSE_WORKSPACE
  $(error No BOSE_WORKSPACE)
endif

sdk = qc8017_32
disableGVA ?= false

# Used to determine what type of BLOB Manifest file to use. Acceptible options
# are Release, Continuous, or Nightly.
BUILD_TYPE=Release

ifeq ($(sdk),$(filter native x86 x86_64, $(sdk)))
  cfg = Debug
else
  cfg = Release
endif


jobs := $(shell grep -c ^processor /proc/cpuinfo)

$(info sdk=${sdk} cfg=${cfg} jobs=${jobs} disableGVA=${disableGVA})

export BUILDS_DIR := $(BOSE_WORKSPACE)/builds/$(cfg)/$(sdk)

PROTO_CC_DEST := $(BUILDS_DIR)/proto
PROTO_PY_DEST := $(BUILDS_DIR)/proto_py

ifeq (CentOS,$(shell lsb_release -is))
  CMAKE = cmake3          # sudo yum install cmake3
else
  CMAKE = cmake
endif
