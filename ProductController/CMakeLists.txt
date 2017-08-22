CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

PROJECT(Professor)

INCLUDE(../Settings.cmake)

SET(PROTO_CC_DEST ${CMAKE_BINARY_DIR}/proto)

FIND_LIBRARY(PROTOBUF_LIB NAMES libprotobuf.so
  PATHS ${BOSELIBS_DIR}/lib
  NO_DEFAULT_PATH
)

INCLUDE_DIRECTORIES(
  ${CMAKE_BINARY_DIR}
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
  ${PROTOBUF_LIB}
)

INCLUDE(exe.cmake)
