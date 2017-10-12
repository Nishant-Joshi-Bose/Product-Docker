INCLUDE_DIRECTORIES(
  ${RIVIERA_LPM_SERVICE_DIR}/source/Client
  ${RIVIERA_LPM_SERVICE_DIR}/source/LPMIPC
  ${RIVIERA_LPM_DIR}/include
  ${CASTLE_PRODUCT_CONTROLLER_COMMON_DIR}
  ${CASTLE_PRODUCT_CONTROLLER_COMMON_DIR}/Hsm
  ${CUSTOM_HSM_DIR}
  ${KEYHANDLER_DIR}/include
  DeviceManager
  LightBarController
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
)

TARGET_LINK_LIBRARIES(Eddie
  CastleProductControllerCommon
  LpmClient
  FrontDoorClient
  NetworkServiceClient
  KeyHandler
  ShelbyUtils
  BoseLibs
  libjsoncpp.a
  libprotobuf.a
  libTinyXml.a
  pthread
  rt
)

INSTALL(TARGETS Eddie DESTINATION ${OUTPUT_BIN_DIR})
