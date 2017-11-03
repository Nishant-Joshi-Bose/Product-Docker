ifndef BOSE_WORKSPACE
  $(error No BOSE_WORKSPACE)
endif

sdk = qc8017_32
ifeq ($(sdk),native)
  cfg = Debug
else
  cfg = Release
endif
jobs = 1

BUILDS_DIR := $(BOSE_WORKSPACE)/builds/$(cfg)/$(sdk)

PROTO_CC_DEST := $(BUILDS_DIR)/proto
PROTO_PY_DEST := $(BUILDS_DIR)/proto_py
