INCLUDE_DIRECTORIES(
  ${RIVIERA_LPM_SERVICE_DIR}/source/Client
  ${RIVIERA_LPM_SERVICE_DIR}/source/LPMIPC
  ${RIVIERA_LPM_DIR}/include
  ${CASTLE_PRODUCT_CONTROLLER_COMMON_DIR}
  ${CASTLE_PRODUCT_CONTROLLER_COMMON_DIR}/Hsm
  CustomHsm
  DeviceManager
  .
)

SET(SOUNDTOUCH_SDK_LIBS
  FrontDoorClient
  ShelbyUtils
  BoseLibs
)

# -----------------------------------------------------------------------------
# Professor daemon.

ADD_EXECUTABLE(Professor
  CustomHsm/CustomProductControllerState.cpp
  CustomHsm/CustomProductControllerStateBooting.cpp
  CustomHsm/CustomProductControllerStateSwUpdating.cpp
  HelloWorldProductController.cpp
  Main.cpp
  ${PROTO_CC_DEST}/Language.pb.cc
)

TARGET_LINK_LIBRARIES(Professor
  CastleProductControllerCommon
  ${SOUNDTOUCH_SDK_LIBS}
  LpmClient
  jsoncpp
  protobuf
  -lpthread
  -lrt
  -ltinyxml
)

INSTALL(TARGETS Professor DESTINATION ${OUTPUT_BIN_DIR})

