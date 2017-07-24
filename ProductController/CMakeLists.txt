CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

INCLUDE(../Settings.cmake)

PROJECT(Eddie)

FIND_LIBRARY(PROTOBUF_LIB NAMES libprotobuf.so
  PATHS ${SOUNDTOUCH_SDK_DIR}/fs/opt/Bose/lib
  NO_DEFAULT_PATH
)

INCLUDE_DIRECTORIES(
  ${CMAKE_BINARY_DIR}
  ${SOUNDTOUCH_SDK_DIR}/prebuilt/include
)

SET(SOUNDTOUCH_SDK_LIBS
  ${SOUNDTOUCH_SDK_DIR}/prebuilt/Libs/libSoundTouchClient.a
  ${SOUNDTOUCH_SDK_DIR}/prebuilt/Libs/libSTS.a
  ${SOUNDTOUCH_SDK_DIR}/prebuilt/Libs/libShelbyUtils.a
  ${PROTOBUF_LIB}
)

INCLUDE(exe.cmake)
INCLUDE(package.cmake)

ADD_SUBDIRECTORY(AlertSubSystem)
ADD_SUBDIRECTORY(LocalUI)
