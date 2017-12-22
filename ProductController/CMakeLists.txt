INCLUDE_DIRECTORIES(
  ${PRODUCTCONTROLLERCOMMON_DIR}
  ${PRODUCTCONTROLLERCOMMON_DIR}/Hsm
  ${PRODUCTCONTROLLERCOMMON_DIR}/ProductSTSController
  ${PRODUCTCONTROLLERCOMMON_DIR}/ProductAudioService
  ${PRODUCTCONTROLLERCOMMON_DIR}/IntentHandler
  ${DEMOCONTROLLER_DIR}
  ${DATACOLLECTIONCLIENT_DIR}
  ${CUSTOM_HSM_DIR}
  ${KEYHANDLER_DIR}/include
  IntentHandler
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
  ${PROTO_CC_DEST}/VolumeConfig.pb.cc
  IntentHandler/IntentHandler.cpp
  IntentHandler/PlaybackRequestManager.cpp
  IntentHandler/VoiceManager.cpp
  DisplayController/DisplayController.cpp
  CustomProductAudioService.cpp
)

TARGET_LINK_LIBRARIES(Eddie
  CastleProductControllerCommon
  DemoController
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
