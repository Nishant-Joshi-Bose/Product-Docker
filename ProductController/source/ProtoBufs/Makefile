include $(BOSE_WORKSPACE)/Settings.mk

BOSELIBS_DIR := $(shell components get BoseLibs-$(sdk) installed_location)
SOUNDTOUCH_SDK_DIR := $(shell components get SoundTouch-SDK-$(sdk) installed_location)

PROTO_SOURCES = \
      AudioSettings.proto           \
      ProductMessage.proto          \
      RebroadcastLatencyMode.proto  \
      A4VPersistence.proto          \
      A4VSystemTimeout.proto        \
      ConfigurationStatus.proto     \
      Language.proto				\
      ContentItem.proto 			\

# Where to search for imported *.proto files.
PROTO_IMPORT_DIRS := \
 . \
 $(PROTO_CC_DEST) \
 $(BOSELIBS_DIR)/protobufs \
 $(BOSELIBS_DIR)/include \
 $(SOUNDTOUCH_SDK_DIR)/protobufs \
 $(SOUNDTOUCH_SDK_DIR)/prebuilt/include \

include $(BOSELIBS_DIR)/protobuf.mk
