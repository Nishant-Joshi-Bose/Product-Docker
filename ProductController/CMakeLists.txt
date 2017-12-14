INCLUDE_DIRECTORIES(
  ${NETWORK_SERVICE_DIR}/include
  ${CASTLECLI_DIR}/include
  ${SOUNDTOUCH_SDK_DIR}/prebuilt/include/SoundTouchInterface
  ${SOUNDTOUCH_SDK_DIR}/prebuilt/include/Bluetooth
  ${PRODUCTCONTROLLERCOMMON_DIR}
  ${PRODUCTCONTROLLERCOMMON_DIR}/Hsm
  ${PRODUCTCONTROLLERCOMMON_DIR}/ProductSTSController
  ${PRODUCTCONTROLLERCOMMON_DIR}/ProductAudioService
  ${PRODUCTCONTROLLERCOMMON_DIR}/IntentHandler
  ${PRODUCTCONTROLLERCOMMON_DIR}/ProductHardwareInterface
  ${KEYHANDLER_DIR}/include
  .
  include
  source
  source/CustomStateMachine
  source/IntentHandler
  source/CustomProductAudioService
  ${A4VVIDEOMANAGER_DIR}/source/Client
)

# -----------------------------------------------------------------------------
# Professor daemon.

ADD_EXECUTABLE(Professor
  ${PROTO_CC_DEST}/ProductMessage.pb.cc
  ${PROTO_CC_DEST}/ConfigurationStatus.pb.cc
  ${PROTO_CC_DEST}/ProductSTS.pb.cc
  ${PROTO_CC_DEST}/KeyActions.pb.cc
  ${PROTO_CC_DEST}/ProductSpeakerManager.pb.cc
  ${PROTO_CC_DEST}/SystemInfo.pb.cc
  ${PROTO_CC_DEST}/AdaptIQ.pb.cc
  ${PROTO_CC_DEST}/AudioSettings.pb.cc
  source/ProductStart.cpp
  source/ProductCommandLine.cpp
  source/CustomProductHardwareInterface.cpp
  source/ProductNetworkManager.cpp
  source/ProductSystemManager.cpp
  source/ProductKeyInputInterface.cpp
  source/ProductSpeakerManager.cpp
  source/ProfessorProductController.cpp
  source/ProductEdidInterface.cpp
  source/ProductVolumeManager.cpp
  source/CustomProductAudioService/CustomProductAudioService.cpp
  source/CustomProductAudioService/CustomAudioSettingsManager.cpp
  source/ProductAdaptIQManager.cpp
  source/CustomStateMachine/CustomProductControllerState.cpp
  source/CustomStateMachine/CustomProductControllerStateAccessoryPairing.cpp
  source/CustomStateMachine/CustomProductControllerStateBooting.cpp
  source/CustomStateMachine/CustomProductControllerStateIdle.cpp
  source/CustomStateMachine/CustomProductControllerStateIdleVoiceConfigured.cpp
  source/CustomStateMachine/CustomProductControllerStateIdleVoiceUnconfigured.cpp
  source/CustomStateMachine/CustomProductControllerStateNetworkStandbyConfigured.cpp
  source/CustomStateMachine/CustomProductControllerStateNetworkStandby.cpp
  source/CustomStateMachine/CustomProductControllerStateNetworkStandbyUnconfigured.cpp
  source/CustomStateMachine/CustomProductControllerStateLowPower.cpp
  source/CustomStateMachine/CustomProductControllerStateOn.cpp
  source/CustomStateMachine/CustomProductControllerStatePlayable.cpp
  source/CustomStateMachine/CustomProductControllerStatePlaying.cpp
  source/CustomStateMachine/CustomProductControllerStatePlayingActive.cpp
  source/CustomStateMachine/CustomProductControllerStatePlayingInactive.cpp
  source/CustomStateMachine/CustomProductControllerStateUpdatingSoftware.cpp
  source/IntentHandler/IntentHandler.cpp
)

TARGET_LINK_LIBRARIES(Professor
  CastleProductControllerCommon
  LpmClient
  A4VVideoManagerClient
  FrontDoorClient
  NetworkServiceClient
  KeyHandler
  BLESetupServiceProto
  BluetoothSinkServiceProto
  LightBarController
  VoiceServiceClient
  CliClient
  ShelbyUtils
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
)

INSTALL(TARGETS Professor DESTINATION ${OUTPUT_BIN_DIR})

# -----------------------------------------------------------------------------
