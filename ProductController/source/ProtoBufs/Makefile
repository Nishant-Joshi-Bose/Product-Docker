include $(BOSE_WORKSPACE)/Settings.mk

BOSELIBS_DIR       := $(shell components get BoseLibs-$(sdk)       installed_location)
SOUNDTOUCH_SDK_DIR := $(shell components get SoundTouch-SDK-$(sdk) installed_location)

PROTO_SOURCES = ProductMessage.proto        \
                ConfigurationStatus.proto   \
                Language.proto              \
                ProductSTS.proto            \
                KeyActions.proto            \
                SystemInfo.proto            \
                ProductSpeakerManager.proto \

#
# The following alias is used to search for imported Protocol Buffer files.
#
PROTO_IMPORT_DIRS :=  .                                       \
                      ../../../ProductSTSController/Protobufs \
                      $(PROTO_CC_DEST)                        \
                      $(BOSELIBS_DIR)/protobufs               \
                      $(BOSELIBS_DIR)/include                 \
                      $(SOUNDTOUCH_SDK_DIR)/protobufs         \
                      $(SOUNDTOUCH_SDK_DIR)/prebuilt/include  \

include $(BOSELIBS_DIR)/protobuf.mk
