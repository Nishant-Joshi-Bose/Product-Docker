include $(BOSE_WORKSPACE)/Settings.mk

PROTO_SOURCES = \
      AudioControls.proto           \
      ProductMessage.proto          \
      RebroadcastLatencyMode.proto  \
      A4VPersistence.proto          \
      A4VSystemTimeout.proto

# Where to write the *.pb.cc and *.pb.h files.
ifndef PROTO_CC_DEST
  $(error No PROTO_CC_DEST)
endif

BOSELIB_DIR := $(shell components get BoseLibs-$(sdk) installed_location)
SOUNDTOUCH_SDK_DIR := $(shell components get SoundTouch-SDK-$(sdk) installed_location)
PROTOC := $(BOSELIB_DIR)/native/bin/protoc
PROTOC_DEPEND := $(BOSELIB_DIR)/native/bin/protoc-depend

# Where to search for imported *.proto files.
PROTO_IMPORT_DIRS := \
 . \
 $(PROTO_CC_DEST)\
 $(BOSELIB_DIR)/protobufs \
 $(BOSELIB_DIR)/include \
 $(SOUNDTOUCH_SDK_DIR)/protobufs \
 $(SOUNDTOUCH_SDK_DIR)/prebuilt/include \

# $1 - one or more *.proto file names.
# returns the set of *.pb.cc and *.pb.h file names
define proto-cc
$(foreach f,$1,$(PROTO_CC_DEST)/$(f:.proto=.pb.cc) $(PROTO_CC_DEST)/$(f:.proto=.pb.h))
endef

PROTO_CC := $(call proto-cc,$(PROTO_SOURCES))
PROTO_CC_DEPENDS := $(foreach f,$(PROTO_SOURCES),$(PROTO_CC_DEST)/$f.d)
-include $(PROTO_CC_DEPENDS)

$(call proto-cc,%.proto): %.proto
	mkdir -p $(dir $@)
	@echo "Building Protocol Buffers Now..."
	$(PROTOC) $(--proto_path=%, $(PROTO_IMPORT_DIRS)) --cpp_out=$(PROTO_CC_DEST) $<

#
# Rules for compiling *.proto files into C++.
#
.PHONY: all
all: build-proto

.PHONY: clean
clean:
	rm -f $(PROTO_CC) $(PROTO_CC_DEPENDS)

.PHONY: build-proto
build-proto: $(PROTO_CC)
