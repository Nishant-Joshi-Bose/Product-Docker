CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

PROJECT(Eddie)

INCLUDE(../Settings.cmake)

INCLUDE_DIRECTORIES(
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
