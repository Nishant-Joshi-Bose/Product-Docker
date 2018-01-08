ifndef BOSE_WORKSPACE
  $(error No BOSE_WORKSPACE)
endif

sdk = qc8017_32

ifeq ($(sdk),$(filter native x86 x86_64, $(sdk)))
  cfg = Debug
else
  cfg = Release
endif

jobs := $(shell grep -c ^processor /proc/cpuinfo)

$(info sdk=${sdk} cfg=${cfg} jobs=${jobs})

export BUILDS_DIR := $(BOSE_WORKSPACE)/builds/$(cfg)/$(sdk)

PROTO_CC_DEST := $(BUILDS_DIR)/proto
PROTO_PY_DEST := $(BUILDS_DIR)/proto_py
