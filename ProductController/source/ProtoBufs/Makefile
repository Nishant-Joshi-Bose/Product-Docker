include $(BOSE_WORKSPACE)/Settings.mk

BOSELIBS_DIR                    := $(shell components get BoseLibs-$(sdk)               installed_location)
SOUNDTOUCH_SDK_DIR              := $(shell components get SoundTouch-SDK-$(sdk)         installed_location)
PRODUCTCONTROLLERCOMMON_DIR	:= $(shell components get CastleProductControllerCommon installed_location)
RIVIERA_LPM_PROTO_DIR 		:= $(shell components get RivieraLpmService-$(sdk)      installed_location)

PROTO_SOURCES = ProductMessage.proto        \
                ConfigurationStatus.proto   \
                LightBarController.proto  	\
                ProductSTS.proto            \
                SystemInfo.proto            \
                ProductSpeakerManager.proto \
                AdaptIQ.proto               \
                AudioSettings.proto         \

#
# The following alias is used to search for imported Protocol Buffer files.
#
PROTO_IMPORT_DIRS :=    .                                         \
                        $(PROTO_CC_DEST)                          \
                        $(BOSELIBS_DIR)/protobufs                 \
                        $(BOSELIBS_DIR)/include                   \
                        $(SOUNDTOUCH_SDK_DIR)/protobufs           \
                        $(SOUNDTOUCH_SDK_DIR)/prebuilt/include    \
                        $(PRODUCTCONTROLLERCOMMON_DIR)/Protobufs  \
                        $(RIVIERA_LPM_PROTO_DIR)/proto            \

include $(BOSELIBS_DIR)/protobuf.mk
