# ToDo Have CMakeLists.txt file instead of this Makefile.
PROTO_SOURCES = \
      ProductMessage.proto          \
      A4VSystemTimeout.proto        \
      AudioControls.proto           \
      ProductMessage.proto          \
      RebroadcastLatencyMode.proto

# Where to write the *.pb.cc and *.pb.h files.
#ifndef PROTO_CC_DEST
#  $(error No PROTO_CC_DEST)
#endif
#PROTO_CC_DEST := ..

#
# Rules for compiling *.proto files into C++.
#

.PHONY: all
all: build-proto

#PROTOC := ${PWD}/builds/SoundTouch-SDK-qc8017_32/prebuilt/Debug/native/bin/protoc
PROTOC := ../../../components/SoundTouch-SDK-qc8017_32/prebuilt/Debug/native/bin/protoc

# Where to search for imported *.proto files.
PROTO_IMPORT_DIRS ?= \
 $(PROTO_CC_DEST)\
 $(SOUNDTOUCH_SDK_DIR)/protobufs \
 $(SOUNDTOUCH_SDK_DIR)/prebuilt/include \
 ProductController/source/ProtoBufs


# $1 - one or more *.proto file names.
# returns the set of *.pb.cc and *.pb.h file names
define proto-cc
$(foreach f,$1,$(PROTO_CC_DEST)/$(f:.proto=.pb.cc) $(PROTO_CC_DEST)/$(f:.proto=.pb.h))
endef

PROTO_CC := $(call proto-cc,$(PROTO_SOURCES))
PROTO_CC_DEPENDS := $(foreach f,$(PROTO_SOURCES),$(PROTO_CC_DEST)/$f.d)
-include $(PROTO_CC_DEPENDS)

#$(call proto-cc,%.proto): %.proto
#	mkdir -p $(dir $@)
#	@echo "Building Protocol Buffers..."
#	$(PROTOC) $(--proto_path=%, $(PROTO_IMPORT_DIRS)) --cpp_out=$(PROTO_CC_DEST) $<

$(call proto-cc,%.proto): %.proto
	mkdir -p $(dir $@)
	@echo "Building Protocol Buffers Now..."
	$(PROTOC) $(--proto_path=%, $(PROTO_IMPORT_DIRS)) --cpp_out=$(PROTO_CC_DEST) $<

.PHONY: clean
clean:
	rm -f $(PROTO_CC) $(PROTO_CC_DEPENDS)

.PHONY: build-proto
build-proto: $(PROTO_CC)
