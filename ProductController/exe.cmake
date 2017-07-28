ADD_EXECUTABLE(Professor
    source/ProductStart.cpp
    source/ProductCommandLine.cpp
    source/ProductHardwareManager.cpp
    source/ProductController.cpp
    source/ProductDeviceManager.cpp
    source/ProductSystemInterface.cpp
    source/ProductUserInterface.cpp
    source/ProductMessage.pb.cc
    source/A4VSystemTimeout.pb.cc
    source/AudioControls.pb.cc
    source/ProductMessage.pb.cc
    source/RebroadcastLatencyMode.pb.cc
)

TARGET_LINK_LIBRARIES(Professor
  ${SOUNDTOUCH_SDK_LIBS}
  -lpthread
  -lrt
  -ltinyxml
)

INSTALL(TARGETS Professor DESTINATION ${OUTPUT_BIN_DIR})
