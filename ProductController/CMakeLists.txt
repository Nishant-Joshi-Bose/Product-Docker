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
  DeviceManager.cpp
  ProductCliClient.cpp
  EddieProductController.cpp
  Main.cpp
  ${PROTO_CC_DEST}/ConfigurationStatus.pb.cc
  ${PROTO_CC_DEST}/Language.pb.cc
  ${PROTO_CC_DEST}/DeviceManager.pb.cc
  ${PROTO_CC_DEST}/ProductMessage.pb.cc
  ${PROTO_CC_DEST}/AudioSettings.pb.cc
  ${PROTO_CC_DEST}/ContentItem.pb.cc
  ${PROTO_CC_DEST}/DisplayController.pb.cc
  ${PROTO_CC_DEST}/IntentHandler.pb.cc
  IntentHandler/IntentHandler.cpp
  IntentHandler/TransportControlManager.cpp
  IntentHandler/BluetoothManager.cpp
  IntentHandler/PlaybackRequestManager.cpp
  LpmInterface.cpp
  DisplayController/DisplayController.cpp
)

TARGET_LINK_LIBRARIES(Eddie
  CastleProductControllerCommon
  CastleDemoController
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
  STS
  libjsoncpp.a
  libprotobuf.a
  libTinyXml.a
  pthread
  rt
)

INSTALL(TARGETS Eddie DESTINATION ${OUTPUT_BIN_DIR})
