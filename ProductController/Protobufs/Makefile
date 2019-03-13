include $(BOSE_WORKSPACE)/Settings.mk

BOSELIBS_DIR := $(shell components get BoseLibs-$(sdk) installed_location)
CAPSAPI_DIR := $(shell components get CAPSAPI-$(sdk) installed_location)
PRODUCTCONTROLLERCOMMONPROTO_DIR := $(shell components get ProductControllerCommonProto-$(sdk) installed_location)
PRODUCTCONTROLLERCOMMON_DIR := $(shell components get ProductControllerCommon installed_location)
RIVIERALPMSERVICE_DIR := $(shell components get RivieraLpmService-$(sdk) installed_location)
SOUNDTOUCH_SDK_DIR := $(shell components get SoundTouch-SDK-$(sdk) installed_location)
SYSTEMEVENTSERVICE_DIR := $(shell components get SystemEventService-$(sdk) installed_location)

PROTO_SOURCES = \
  AudioSettings.proto \
  Intents.proto \
  KeyActions.proto \
  KeyNames.proto \
  LightBarController.proto \
  ProductMessage.proto \
  ProductSTS.proto \

PROTO_IMPORT_DIRS := \
 . \
 $(BOSELIBS_DIR)/protobufs \
 $(CAPSAPI_DIR)/SoundTouchInterfaceAPI/protobufs \
 $(PRODUCTCONTROLLERCOMMONPROTO_DIR)/protobufs \
 $(PRODUCTCONTROLLERCOMMON_DIR)/Protobufs/private \
 $(RIVIERALPMSERVICE_DIR)/proto \
 $(SOUNDTOUCH_SDK_DIR)/protobufs \
 $(SYSTEMEVENTSERVICE_DIR)/protobufs \

include $(BOSELIBS_DIR)/protobuf.mk
