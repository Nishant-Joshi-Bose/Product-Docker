ifndef BOSE_WORKSPACE
  $(error No BOSE_WORKSPACE)
endif

sdk = qc8017_32

ifeq ($(sdk),$(filter native x86 x86_64, $(sdk)))
  cfg = Debug
else
  cfg = Release
endif

ifeq ($(DEPLOY_ENV),$(filter dev DEV, $(DEPLOY_ENV)))
	privateKeyFilePath="$(PWD)/scripts/keys/development/privateKey/dev.p12"
	privateKeyPasswordPath="$(PWD)/scripts/keys/development/privateKey/dev_p12.pass"
else
        #todo : handle production case
	privateKeyFilePath="prod"
	privateKeyPasswordPath="prod"
endif

jobs := $(shell grep -c ^processor /proc/cpuinfo)

$(info sdk=${sdk} cfg=${cfg} jobs=${jobs})

BUILDS_DIR := $(BOSE_WORKSPACE)/builds/$(cfg)/$(sdk)

PROTO_CC_DEST := $(BUILDS_DIR)/proto

