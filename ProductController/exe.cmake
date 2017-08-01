ADD_EXECUTABLE(Professor
    source/ProductStart.cpp
    source/ProductCommandLine.cpp
    source/ProductHardwareManager.cpp
    source/ProductController.cpp
    source/ProductDeviceManager.cpp
    source/ProductSystemInterface.cpp
    source/ProductUserInterface.cpp
    ${PROTO_CC_DEST}/A4VSystemTimeout.pb.cc
    ${PROTO_CC_DEST}/AudioControls.pb.cc
    ${PROTO_CC_DEST}/ProductMessage.pb.cc
    ${PROTO_CC_DEST}/RebroadcastLatencyMode.pb.cc
)

TARGET_LINK_LIBRARIES(Professor
  ${SOUNDTOUCH_SDK_LIBS}
  -lpthread
  -lrt
  -ltinyxml
)

INSTALL(TARGETS Professor DESTINATION ${OUTPUT_BIN_DIR})
