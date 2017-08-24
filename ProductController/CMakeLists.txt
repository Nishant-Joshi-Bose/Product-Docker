INCLUDE_DIRECTORIES(
  ${CMAKE_BINARY_DIR}
  ${CMAKE_BINARY_DIR}/proto
  ${BOSELIBS_DIR}/include
  ${SOUNDTOUCH_SDK_DIR}/prebuilt/include
  ${SOUNDTOUCH_SDK_DIR}/prebuilt/include/STS
  ${SOUNDTOUCH_SDK_DIR}/prebuilt/include/SoundTouchInterface
  ${PROTO_CC_DEST}
  include
)

SET(SOUNDTOUCH_SDK_LIBS
  ${SOUNDTOUCH_SDK_DIR}/prebuilt/Libs/libSoundTouchClient.a
  ${SOUNDTOUCH_SDK_DIR}/prebuilt/Libs/libShelbyUtils.a
  ${BOSELIBS_DIR}/lib/libBoseLibs.a
)

# -----------------------------------------------------------------------------
# Professor daemon.

ADD_EXECUTABLE(Professor
  source/ProductStart.cpp
  source/ProductCommandLine.cpp
  source/ProductHardwareManager.cpp
  source/ProductController.cpp
  source/ProductDeviceManager.cpp
  source/ProductSystemInterface.cpp
  source/ProductUserInterface.cpp
  ${PROTO_CC_DEST}/A4VPersistence.pb.cc
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

# -----------------------------------------------------------------------------
