include $(BOSE_WORKSPACE)/Settings.mk

BOSELIBS_DIR                := $(shell components get BoseLibs-$(sdk) installed_location)
PRODUCTCONTROLLERCOMMON_DIR := $(shell components get CastleProductControllerCommon installed_location)
RIVIERA_LPM_PROTO_DIR       := $(shell components get RivieraLpmService-$(sdk) installed_location)
SOUNDTOUCH_SDK_DIR          := $(shell components get SoundTouch-SDK-$(sdk)         installed_location)
SYSTEMEVENTSERVICE_DIR      := $(shell components get SystemEventService-$(sdk) installed_location)

PROTO_SOURCES = \
  ConfigurationStatus.proto \
  LightBarController.proto  \
  ProductMessage.proto      \
  KeyActions.proto          \
  DisplayController.proto   \
  AudioSettings.proto       \

# Where to search for imported *.proto files.
PROTO_IMPORT_DIRS := \
 . \
 $(PROTO_CC_DEST) \
 $(BOSELIBS_DIR)/protobufs \
 $(PRODUCTCONTROLLERCOMMON_DIR)/Protobufs \
 $(RIVIERA_LPM_PROTO_DIR)/proto \
 $(SOUNDTOUCH_SDK_DIR)/protobufs \
 $(SOUNDTOUCH_SDK_DIR)/prebuilt/include \
 $(SYSTEMEVENTSERVICE_DIR)/protobufs \

include $(BOSELIBS_DIR)/protobuf.mk
