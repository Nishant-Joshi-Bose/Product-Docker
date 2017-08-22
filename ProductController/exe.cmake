SET( PROTOBUFS_SRCS
    ${PROTO_CC_DEST}/ConfigurationStatus.pb.cc
    ${PROTO_CC_DEST}/Language.pb.cc
)

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
  ${PROTOBUFS_SRCS}
  ${CMAKE_BINARY_DIR}/proto/AutoLpmServiceMessages.pb.cc
  ${CMAKE_BINARY_DIR}/proto/LpmServiceMessages.pb.cc
  ${RIVIERA_LPM_SERVICE_DIR}/source/Client/LpmClientFactory.cpp
  ${RIVIERA_LPM_SERVICE_DIR}/source/Client/LpmClient.cpp
  ${RIVIERA_LPM_SERVICE_DIR}/source/Client/AutoLpmClient.cpp
)

TARGET_LINK_LIBRARIES(Eddie
  ${SOUNDTOUCH_SDK_LIBS}
  -lpthread
  -lrt
  -ltinyxml
)

INSTALL(TARGETS Eddie DESTINATION ${OUTPUT_BIN_DIR})
