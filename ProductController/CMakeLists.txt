INCLUDE_DIRECTORIES(
  ${PRODUCTCONTROLLERCOMMON_DIR}
  ${PRODUCTCONTROLLERCOMMON_DIR}/Hsm
  ${DEMOCONTROLLER_DIR}
  ${CUSTOM_HSM_DIR}
  ${KEYHANDLER_DIR}/include
  DeviceManager
  LightBarController
  IntentHandler
  .
)

# -----------------------------------------------------------------------------
# Eddie daemon.

ADD_EXECUTABLE(Eddie
  ${CUSTOM_HSM_DIR}/CustomProductControllerState.cpp
  ${CUSTOM_HSM_DIR}/EddieProductControllerHsm.cpp
  ${CUSTOM_HSM_DIR}/EddieProductControllerStateTop.cpp
  ${CUSTOM_HSM_DIR}/EddieProductControllerStateBooting.cpp
  ${CUSTOM_HSM_DIR}/EddieProductControllerStateSetup.cpp
  ${CUSTOM_HSM_DIR}/EddieProductControllerStateNetworkStandby.cpp
  DeviceManager.cpp
  LightBarController/LightBarController.cpp
  ProductCliClient.cpp
  ProductSource.cpp
  EddieProductController.cpp
  Main.cpp
  ${PROTO_CC_DEST}/ConfigurationStatus.pb.cc
  ${PROTO_CC_DEST}/Language.pb.cc
  ${PROTO_CC_DEST}/DeviceManager.pb.cc
  ${PROTO_CC_DEST}/LightBarController.pb.cc
  ${PROTO_CC_DEST}/ProductMessage.pb.cc
  ${PROTO_CC_DEST}/AudioSettings.pb.cc
  ${PROTO_CC_DEST}/ContentItem.pb.cc
  IntentHandler/IntentHandler.cpp
  IntentHandler/TransportControlManager.cpp
  IntentHandler/BlueToothManager.cpp
  LpmInterface.cpp
)

TARGET_LINK_LIBRARIES(Eddie
  CastleProductControllerCommon
  CastleDemoController
  LpmClient
  FrontDoorClient
  NetworkServiceClient
  KeyHandler
  BLESetupServiceProto
  CliClient
  BoseLibs
  libjsoncpp.a
  libprotobuf.a
  libTinyXml.a
  pthread
  rt
)

INSTALL(TARGETS Eddie DESTINATION ${OUTPUT_BIN_DIR})
