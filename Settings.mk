ifndef WORKSPACE
  $(error No WORKSPACE)
endif

sdk = qc8017_32
ifeq ($(sdk),native)
  cfg = Debug
else
  cfg = Release
endif
jobs = 1

BUILDS_DIR := $(WORKSPACE)/builds/$(cfg)/$(sdk)