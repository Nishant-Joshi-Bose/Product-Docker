INCLUDE_DIRECTORIES(
  ${RIVIERA_LPM_SERVICE_DIR}/source/Client
  ${RIVIERA_LPM_SERVICE_DIR}/source/LPMIPC
  ${RIVIERA_LPM_DIR}/include
  ${CASTLE_PRODUCT_CONTROLLER_COMMON_DIR}
  ${CASTLE_PRODUCT_CONTROLLER_COMMON_DIR}/Hsm
  ${CUSTOM_HSM_DIR}
  DeviceManager
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
  ProductCliClient.cpp
  EddieProductController.cpp
  Main.cpp
  ${PROTO_CC_DEST}/ConfigurationStatus.pb.cc
  ${PROTO_CC_DEST}/Language.pb.cc
  ${PROTO_CC_DEST}/DeviceManager.pb.cc
)

TARGET_LINK_LIBRARIES(Eddie
  CastleProductControllerCommon
  ${SOUNDTOUCH_SDK_LIBS}
  LpmClient
  FrontDoorClient
  ShelbyUtils
  BoseLibs
  jsoncpp
  protobuf
  tinyxml
  pthread
  rt
)

INSTALL(TARGETS Eddie DESTINATION ${OUTPUT_BIN_DIR})

# -----------------------------------------------------------------------------

ADD_SUBDIRECTORY(LocalUI)
