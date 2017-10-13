include $(BOSE_WORKSPACE)/Settings.mk

BOSELIBS_DIR := $(shell components get BoseLibs-$(sdk) installed_location)

PROTO_SOURCES = \
  ProductSTSIntrospect.proto \
  ProductSTS.proto \

# Where to search for imported *.proto files.
PROTO_IMPORT_DIRS := \
 . \
 $(PROTO_CC_DEST) \
 $(BOSELIBS_DIR)/protobufs \
 $(BOSELIBS_DIR)/include \

include $(BOSELIBS_DIR)/protobuf.mk

