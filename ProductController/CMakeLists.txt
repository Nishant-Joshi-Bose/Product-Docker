INCLUDE_DIRECTORIES(
  ${A4VVIDEOMANAGER_DIR}/source/Client
  ${A4VQUICKSET_DIR}/source/Client
  ${CASTLECLI_DIR}/include
  ${KEYHANDLER_DIR}/include
  ${NETWORK_SERVICE_DIR}/include
  ${PRODUCTCONTROLLERCOMMON_DIR}
  ${PRODUCTCONTROLLERCOMMON_DIR}/ChimesController
  ${PRODUCTCONTROLLERCOMMON_DIR}/Hsm
  ${PRODUCTCONTROLLERCOMMON_DIR}/IntentHandler
  ${PRODUCTCONTROLLERCOMMON_DIR}/ProductAudioService
  ${PRODUCTCONTROLLERCOMMON_DIR}/ProductLpmHardwareInterface
  ${PRODUCTCONTROLLERCOMMON_DIR}/ProductSTSController
  ${SOUNDTOUCH_SDK_DIR}/prebuilt/include/Bluetooth
  ${SOUNDTOUCH_SDK_DIR}/prebuilt/include/SoundTouchInterface
  .
  include
  source
  source/CustomProductAudioService
  source/CustomProductLpmHardwareInterface
  source/CustomStateMachine
  source/IntentHandler
)

# -----------------------------------------------------------------------------
# Professor daemon.

ADD_EXECUTABLE(Professor
  ${PROTO_CC_DEST}/AdaptIQ.pb.cc
  ${PROTO_CC_DEST}/CecMode.pb.cc
  ${PROTO_CC_DEST}/AudioSettings.pb.cc
  ${PROTO_CC_DEST}/Blaster.pb.cc
  ${PROTO_CC_DEST}/ConfigurationStatus.pb.cc
  ${PROTO_CC_DEST}/ProductMessage.pb.cc
  ${PROTO_CC_DEST}/ProductSpeakerManager.pb.cc
  ${PROTO_CC_DEST}/ProductSTS.pb.cc
  ${PROTO_CC_DEST}/SystemInfo.pb.cc
  ${PROTO_CC_DEST}/SystemSourcesProperties.pb.cc
  source/ProductAdaptIQManager.cpp
  source/ProductCommandLine.cpp
  source/ProductCecHelper.cpp
  source/ProductKeyInputInterface.cpp
  source/ProductNetworkManager.cpp
  source/ProductStart.cpp
  source/ProductSystemManager.cpp
  source/ProfessorProductController.cpp
  source/CustomProductAudioService/CustomAudioSettingsManager.cpp
  source/CustomProductAudioService/CustomProductAudioService.cpp
  source/CustomProductLpmHardwareInterface/CustomProductLpmHardwareInterface.cpp
  source/CustomStateMachine/CustomProductControllerState.cpp
  source/CustomStateMachine/CustomProductControllerStateAccessoryPairing.cpp
  source/CustomStateMachine/CustomProductControllerStateAdaptIQ.cpp
  source/CustomStateMachine/CustomProductControllerStateBooting.cpp
  source/CustomStateMachine/CustomProductControllerStateOn.cpp
  source/CustomStateMachine/CustomProductControllerStatePlayable.cpp
  source/CustomStateMachine/CustomProductControllerStatePlaying.cpp
  source/CustomStateMachine/CustomProductControllerStatePlayingActive.cpp
  source/CustomStateMachine/CustomProductControllerStatePlayingInactive.cpp
  source/IntentHandler/IntentHandler.cpp
  source/IntentHandler/PlaybackRequestManager.cpp
  source/IntentHandler/SpeakerPairingManager.cpp
  source/IntentHandler/MuteManager.cpp
)

TARGET_LINK_LIBRARIES(Professor
  CastleProductControllerCommon
  LpmClient
  A4VVideoManagerClient
  A4VQuickSetServiceClient
  FrontDoorClient
  NetworkServiceClient
  KeyHandler
  BLESetupServiceProto
  BluetoothSinkServiceProto
  LightBarController
  VoiceServiceClient
  CliClient
  ShelbyUtils
  AudioLibs
  BoseLibs
  PassportClient
  STS
  jsoncpp
  protobuf
  TinyXml
  pthread
  rt
  cap
  dl
  SoundTouch_SDK_Protobuf_static
  SoftwareUpdateAPI
  SASSFrontDoorServiceProto
)

INSTALL(TARGETS Professor DESTINATION ${OUTPUT_BIN_DIR})

# -----------------------------------------------------------------------------
