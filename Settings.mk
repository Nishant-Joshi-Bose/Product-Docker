ifndef BOSE_WORKSPACE
  $(error No BOSE_WORKSPACE)
endif

sdk = qc8017_32

# Used to determine what type of BLOB Manifest file to use. Acceptible options
# are Release, Continuous, or Nightly.
BUILD_TYPE=Release

ifeq ($(sdk),$(filter native x86 x86_64, $(sdk)))
  cfg = Debug
else
  cfg = Release
endif

ifeq ($(hw_var),$(filter DP2 Alpha, $(hw_var)))
  hw_var = DP2
endif

jobs := $(shell grep -c ^processor /proc/cpuinfo)

$(info sdk=${sdk} cfg=${cfg} jobs=${jobs} hw_var=${hw_var})

export BUILDS_DIR := $(BOSE_WORKSPACE)/builds/$(cfg)/$(sdk)
export HW_VAR := $(hw_var)

PROTO_CC_DEST := $(BUILDS_DIR)/proto
PROTO_PY_DEST := $(BUILDS_DIR)/proto_py
