include $(BOSE_WORKSPACE)/Settings.mk

BOSELIBS_DIR := $(shell components get BoseLibs-$(sdk) installed_location)

PROTO_SOURCES = \
  ConfigurationStatus.proto \
  LightBarController.proto \
  ProductMessage.proto \
  KeyActions.proto \
  AudioSettings.proto \
  ContentItem.proto \
  DisplayController.proto \
  Presets.proto \

# Where to search for imported *.proto files.
PROTO_IMPORT_DIRS := \
 . \
 $(PROTO_CC_DEST) \
 $(BOSELIBS_DIR)/protobufs \

include $(BOSELIBS_DIR)/protobuf.mk
