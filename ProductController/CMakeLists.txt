CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

INCLUDE(../Settings.cmake)

PROJECT(Professor)

FIND_LIBRARY(PROTOBUF_LIB NAMES libprotobuf.so
  PATHS ${SOUNDTOUCH_SDK_DIR}/fs/opt/Bose/lib
  NO_DEFAULT_PATH
)

INCLUDE_DIRECTORIES(
  ${CMAKE_BINARY_DIR}
  ${BOSELIBS_DIR}/include
  ${SOUNDTOUCH_SDK_DIR}/prebuilt/include
  ${SOUNDTOUCH_SDK_DIR}/prebuilt/include/STS
  include
)

SET(SOUNDTOUCH_SDK_LIBS
  ${SOUNDTOUCH_SDK_DIR}/prebuilt/Libs/libSoundTouchClient.a
  ${SOUNDTOUCH_SDK_DIR}/prebuilt/Libs/libShelbyUtils.a
  ${BOSELIBS_DIR}/libBoseLibs.a
  ${PROTOBUF_LIB}
)

INCLUDE(exe.cmake)
