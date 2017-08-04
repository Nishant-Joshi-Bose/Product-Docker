ADD_EXECUTABLE(Eddie
  Main.cpp
  ProductController.cpp
  FrontDoorClientInterface/FrontDoorClientInterface.cpp
  Hsm/ProductAppState.cpp
  Hsm/ProductAppHsm.cpp
  Hsm/ProductAppStateTop.cpp
  Hsm/ProductAppStateBooting.cpp
  Hsm/ProductAppStateStdOp.cpp
  Hsm/ProductAppStateSetup.cpp
  Hsm/ProductAppStateStandby.cpp
)

TARGET_LINK_LIBRARIES(Eddie
  ${SOUNDTOUCH_SDK_LIBS}
  -lpthread
  -lrt
  -ltinyxml
)

INSTALL(TARGETS Eddie DESTINATION ${OUTPUT_BIN_DIR})
