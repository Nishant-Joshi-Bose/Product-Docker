INCLUDE_DIRECTORIES(
  ${RIVIERA_LPM_SERVICE_DIR}/source/Client
  ${RIVIERA_LPM_SERVICE_DIR}/source/LPMIPC
  ${RIVIERA_LPM_DIR}/include
  Hsm
  .
)

# -----------------------------------------------------------------------------
# Eddie daemon.

ADD_EXECUTABLE(Eddie
  Main.cpp
  ProductController.cpp
  ProductCliClient.cpp
  Hsm/ProductAppState.cpp
  Hsm/ProductAppHsm.cpp
  Hsm/ProductAppStateTop.cpp
  Hsm/ProductAppStateBooting.cpp
  Hsm/ProductAppStateStdOp.cpp
  Hsm/ProductAppStateSetup.cpp
  Hsm/ProductAppStateStandby.cpp
  DeviceManager.cpp
  ${PROTO_CC_DEST}/ConfigurationStatus.pb.cc
  ${PROTO_CC_DEST}/Language.pb.cc
  ${PROTO_CC_DEST}/DeviceManager.pb.cc
)

TARGET_LINK_LIBRARIES(Eddie
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
