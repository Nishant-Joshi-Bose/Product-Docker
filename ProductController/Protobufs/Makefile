include $(BOSE_WORKSPACE)/Settings.mk

BOSELIBS_DIR                     := $(shell components get BoseLibs-$(sdk)                     installed_location)
CAPSAPI_DIR                      := $(shell components get CAPSAPI-$(sdk)                      installed_location)
PRODUCTCONTROLLERCOMMON_DIR	 := $(shell components get ProductControllerCommon             installed_location)
RIVIERA_LPM_PROTO_DIR 		 := $(shell components get RivieraLpmService-$(sdk)            installed_location)
SYSTEMEVENTSERVICE_DIR           := $(shell components get SystemEventService-$(sdk)           installed_location)
AUDIOSOURCE_DIR                  := $(shell components get AudioSource-$(sdk)                  installed_location)
PRODUCTCONTROLLERCOMMONPROTO_DIR := $(shell components get ProductControllerCommonProto-$(sdk) installed_location)

PROTO_SOURCES = ProductMessage.proto          \
                ProductSTS.proto              \
                SystemInfo.proto              \
                ProductSpeakerManager.proto   \
                AdaptIQ.proto                 \
                AudioSettings.proto           \
                AudioFormat.proto             \
                SystemSourcesProperties.proto \
                CecMode.proto                 \
                HdmiEdid.proto                \
                SystemPowerProduct.proto      \
                KeyNames.proto                \
                SystemPowerMacro.proto        \
                DataCollectionCecState.proto  \
                Kepler.proto                  \
                KeyFilter.proto               \
                Intents.proto                 \
                ProductTelemetry.proto        \

#
# The following alias is used to search for imported Protocol Buffer files.
#
PROTO_IMPORT_DIRS :=    .                                         \
                        $(PROTO_CC_DEST)                          \
                        $(BOSELIBS_DIR)/protobufs                 \
                        $(BOSELIBS_DIR)/include                   \
                        $(CAPSAPI_DIR)/SoundTouchInterfaceAPI/protobufs \
                        $(PRODUCTCONTROLLERCOMMON_DIR)/Protobufs/private  \
                        $(PRODUCTCONTROLLERCOMMONPROTO_DIR)/protobufs  \
                        $(RIVIERA_LPM_PROTO_DIR)/proto            \
                        $(SYSTEMEVENTSERVICE_DIR)/protobufs       \
                        $(AUDIOSOURCE_DIR)/proto                  \

include $(BOSELIBS_DIR)/protobuf.mk
