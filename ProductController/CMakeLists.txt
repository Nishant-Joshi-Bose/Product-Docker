INCLUDE_DIRECTORIES(
  ${NETWORK_SERVICE_DIR}/include
  ${CASTLECLI_DIR}/include
  ${SOUNDTOUCH_SDK_DIR}/prebuilt/include/SoundTouchInterface
  ${PRODUCTCONTROLLERCOMMON_DIR}
  ${PRODUCTCONTROLLERCOMMON_DIR}/Hsm
  ${PRODUCTCONTROLLERCOMMON_DIR}/ProductSTSController
  ${PRODUCTCONTROLLERCOMMON_DIR}/ProductAudioService
  ${KEYHANDLER_DIR}/include
  .
  include
  source
  source/CustomStateMachine
  ${A4VVIDEOMANAGER_DIR}/source/Client
)

# -----------------------------------------------------------------------------
# Professor daemon.

ADD_EXECUTABLE(Professor
  ${PROTO_CC_DEST}/AudioSettings.pb.cc
  ${PROTO_CC_DEST}/ProductMessage.pb.cc
  ${PROTO_CC_DEST}/ConfigurationStatus.pb.cc
  ${PROTO_CC_DEST}/Language.pb.cc
  ${PROTO_CC_DEST}/ContentItem.pb.cc
  ${PROTO_CC_DEST}/ProductSTS.pb.cc
  ${PROTO_CC_DEST}/KeyActions.pb.cc
  ${PROTO_CC_DEST}/SystemInfo.pb.cc
  source/ProductStart.cpp
  source/ProductCommandLine.cpp
  source/ProductHardwareInterface.cpp
  source/ProductSoftwareServices.cpp
  source/ProductNetworkManager.cpp
  source/ProductSystemManager.cpp
  source/ProductUserInterface.cpp
  source/ProfessorProductController.cpp
  source/ProductEdidInterface.cpp
  source/CustomStateMachine/CustomProductControllerState.cpp
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
)

TARGET_LINK_LIBRARIES(Professor
  CastleProductControllerCommon
  LpmClient
  KeyHandler
  A4VVideoManagerClient
  SoundTouchClient
  FrontDoorClient
  CliClient
  BoseLibs
  NetworkServiceUtils
  NetworkServiceClient
  STS
  ShelbyUtils
  jsoncpp
  protobuf
  TinyXml
  pthread
  rt
)

INSTALL(TARGETS Professor DESTINATION ${OUTPUT_BIN_DIR})

# -----------------------------------------------------------------------------
