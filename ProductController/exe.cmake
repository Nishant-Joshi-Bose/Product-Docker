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
)

TARGET_LINK_LIBRARIES(Eddie
  ${SOUNDTOUCH_SDK_LIBS}
  ${CMAKE_BINARY_DIR}/libLpmClient.a
  -lpthread
  -lrt
  -ltinyxml
)

INSTALL(TARGETS Eddie DESTINATION ${OUTPUT_BIN_DIR})
