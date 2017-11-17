INCLUDE_DIRECTORIES(
  ${PRODUCTCONTROLLERCOMMON_DIR}
  ${PRODUCTCONTROLLERCOMMON_DIR}/Hsm
  ${PRODUCTCONTROLLERCOMMON_DIR}/ProductSTSController
  ${DEMOCONTROLLER_DIR}
  ${CUSTOM_HSM_DIR}
  ${KEYHANDLER_DIR}/include
  DeviceManager
  IntentHandler
  DisplayController
  .
)

# -----------------------------------------------------------------------------
# Eddie daemon.

ADD_EXECUTABLE(Eddie
  ${CUSTOM_HSM_DIR}/CustomProductControllerState.cpp
  ${CUSTOM_HSM_DIR}/EddieProductControllerStateTop.cpp
  ${CUSTOM_HSM_DIR}/EddieProductControllerStateBooting.cpp
  ${CUSTOM_HSM_DIR}/EddieProductControllerStateSetup.cpp
  ${CUSTOM_HSM_DIR}/EddieProductControllerStateNetworkStandby.cpp
  ${CUSTOM_HSM_DIR}/EddieProductControllerStateAudioOn.cpp
  ProductCliClient.cpp
  EddieProductController.cpp
  Main.cpp
  ${PROTO_CC_DEST}/ConfigurationStatus.pb.cc
  ${PROTO_CC_DEST}/Language.pb.cc
  ${PROTO_CC_DEST}/ProductMessage.pb.cc
  ${PROTO_CC_DEST}/AudioSettings.pb.cc
  ${PROTO_CC_DEST}/ContentItem.pb.cc
  ${PROTO_CC_DEST}/DisplayController.pb.cc
  ${PROTO_CC_DEST}/IntentHandler.pb.cc
  IntentHandler/IntentHandler.cpp
  IntentHandler/TransportControlManager.cpp
  IntentHandler/BluetoothManager.cpp
  IntentHandler/PlaybackRequestManager.cpp
  IntentHandler/NetworkStandbyManager.cpp
  IntentHandler/PresetManager.cpp
  LpmInterface.cpp
  DisplayController/DisplayController.cpp
)

TARGET_LINK_LIBRARIES(Eddie
  CastleProductControllerCommon
  DemoController
  LpmClient
  FrontDoorClient
  NetworkServiceClient
  KeyHandler
  BLESetupServiceProto
  BluetoothSinkServiceProto
  LightBarController
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
)

INSTALL(TARGETS Eddie DESTINATION ${OUTPUT_BIN_DIR})
