CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

INCLUDE(../Settings.cmake)

PROJECT(Eddie)

INCLUDE_DIRECTORIES(
  ${CMAKE_BINARY_DIR}
  ${CMAKE_BINARY_DIR}/proto
  ${BOSELIBS_DIR}/include
  ${SOUNDTOUCH_SDK_DIR}/prebuilt/include
  ${SOUNDTOUCH_SDK_DIR}/prebuilt/include/STS
  ${RIVIERA_LPM_SERVICE_DIR}/source/Client
  ${RIVIERA_LPM_SERVICE_DIR}/source/LPMIPC
  ${RIVIERA_LPM_DIR}/include
  FrontDoorClientInterface
  Hsm
  .
)

SET(SOUNDTOUCH_SDK_LIBS
  ${SOUNDTOUCH_SDK_DIR}/prebuilt/Libs/libSoundTouchClient.a
  ${SOUNDTOUCH_SDK_DIR}/prebuilt/Libs/libShelbyUtils.a
  ${BOSELIBS_DIR}/lib/libBoseLibs.a
)

INCLUDE(exe.cmake)

ADD_SUBDIRECTORY(AlertSubSystem)
ADD_SUBDIRECTORY(LocalUI)
