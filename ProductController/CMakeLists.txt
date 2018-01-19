INCLUDE_DIRECTORIES(
  ${PRODUCTCONTROLLERCOMMON_DIR}
  ${PRODUCTCONTROLLERCOMMON_DIR}/Hsm
  ${PRODUCTCONTROLLERCOMMON_DIR}/ProductSTSController
  ${PRODUCTCONTROLLERCOMMON_DIR}/ProductAudioService
  ${PRODUCTCONTROLLERCOMMON_DIR}/ProductLpmHardwareInterface
  ${PRODUCTCONTROLLERCOMMON_DIR}/IntentHandler
  ${DATACOLLECTIONCLIENT_DIR}
  ${CUSTOM_HSM_DIR}
  ${KEYHANDLER_DIR}/include
  IntentHandler
  CustomProductAudioService
  DisplayController
  .
)

# -----------------------------------------------------------------------------
# Eddie daemon.

ADD_EXECUTABLE(Eddie
  ${CUSTOM_HSM_DIR}/CustomProductControllerState.cpp
  ${CUSTOM_HSM_DIR}/CustomProductControllerStateBooting.cpp
  ${CUSTOM_HSM_DIR}/CustomProductControllerStateSetup.cpp
  ${CUSTOM_HSM_DIR}/CustomProductControllerStateOn.cpp
  ProductCliClient.cpp
  EddieProductController.cpp
  Main.cpp
  ${PROTO_CC_DEST}/ConfigurationStatus.pb.cc
  ${PROTO_CC_DEST}/ProductMessage.pb.cc
  ${PROTO_CC_DEST}/KeyActions.pb.cc
  ${PROTO_CC_DEST}/DisplayController.pb.cc
  ${PROTO_CC_DEST}/IntentHandler.pb.cc
  ${PROTO_CC_DEST}/AudioSettings.pb.cc
  ${PROTO_CC_DEST}/VolumeConfig.pb.cc
  IntentHandler/IntentHandler.cpp
  IntentHandler/CountDownManager.cpp
  IntentHandler/PlaybackRequestManager.cpp
  DisplayController/DisplayController.cpp
  CustomProductAudioService/CustomProductAudioService.cpp
  CustomProductAudioService/CustomAudioSettingsManager.cpp
  CustomProductLpmHardwareInterface.cpp
)

TARGET_LINK_LIBRARIES(Eddie
  CastleProductControllerCommon
  DataCollectionClient
  LpmClient
  FrontDoorClient
  NetworkServiceClient
  KeyHandler
  BLESetupServiceProto
  BluetoothSinkServiceProto
  LightBarController
  VoiceServiceClient
  CliClient
  AudioLibs
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
  SoundTouch_SDK_Protobuf_static
)

INSTALL(TARGETS Eddie DESTINATION ${OUTPUT_BIN_DIR})
