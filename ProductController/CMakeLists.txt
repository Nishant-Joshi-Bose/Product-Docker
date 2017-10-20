MESSAGE( STATUS "ProductController CUSTOM_HSM_DIR                       : " ${CUSTOM_HSM_DIR} )
MESSAGE( STATUS "ProductController PROTO_CC_DEST                        : " ${PROTO_CC_DEST} )
MESSAGE( STATUS "ProductController NETWORK_COMMON_LIBS                  : " ${NETWORK_COMMON_LIBS} )
MESSAGE( STATUS "ProductController NETWORK_COMMON_INCLUDE               : " ${NETWORK_COMMON_INCLUDE} )
MESSAGE( STATUS "ProductController SOUNDTOUCH_SDK_DIR                   : " ${SOUNDTOUCH_SDK_DIR} )
MESSAGE( STATUS "ProductController RIVIERA_LPM_DIR                      : " ${RIVIERA_LPM_DIR} )
MESSAGE( STATUS "ProductController RIVIERA_LPM_SERVICE_DIR              : " ${RIVIERA_LPM_SERVICE_DIR} )
MESSAGE( STATUS "ProductController OUTPUT_BIN_DIR                       : " ${OUTPUT_BIN_DIR} )
MESSAGE( STATUS "ProductController CASTLE_PRODUCT_CONTROLLER_COMMON_DIR : " ${CASTLE_PRODUCT_CONTROLLER_COMMON_DIR} )

INCLUDE_DIRECTORIES(
  ${NETWORK_COMMON_INCLUDE}
  ${CASTLECLI_DIR}/include
  ${SOUNDTOUCH_SDK_DIR}/prebuilt/include/SoundTouchInterface
  ${RIVIERA_LPM_DIR}/include
  ${RIVIERA_LPM_SERVICE_DIR}/source/Client
  ${RIVIERA_LPM_SERVICE_DIR}/source/LPMIPC
  ${RIVIERA_LPM_DIR}/include
  ${CASTLE_PRODUCT_CONTROLLER_COMMON_DIR}
  ${CASTLE_PRODUCT_CONTROLLER_COMMON_DIR}/Hsm
  ${CASTLE_PRODUCT_CONTROLLER_COMMON_DIR}/ProductSTSController
  ${CASTLE_PRODUCT_CONTROLLER_COMMON_DIR}/ProductAudioService
  ${KEYHANDLER_DIR}/include
  .
  include
  source
  source/CustomStateMachine
)


SET(SOUNDTOUCH_SDK_LIBS
  SoundTouchClient
  FrontDoorClient
  CliClient
  ShelbyUtils
  BoseLibs
  ${NETWORK_COMMON_LIBS}/libNetworkServiceUtils.a
  ${NETWORK_COMMON_LIBS}/libNetworkServiceClient.a
  STS
)

# -----------------------------------------------------------------------------
# Professor daemon.

ADD_EXECUTABLE(Professor
  ${PROTO_CC_DEST}/AudioSettings.pb.cc
  ${PROTO_CC_DEST}/ProductMessage.pb.cc
  ${PROTO_CC_DEST}/ConfigurationStatus.pb.cc
  ${PROTO_CC_DEST}/Language.pb.cc
  ${PROTO_CC_DEST}/ContentItem.pb.cc
  ${PROTO_CC_DEST}/ProductSTS.pb.cc
  source/ProductStart.cpp
  source/ProductCommandLine.cpp
  source/ProductHardwareInterface.cpp
  source/ProductSoftwareServices.cpp
  source/ProductNetworkManager.cpp
  source/ProductSystemManager.cpp
  source/ProductUserInterface.cpp
  source/ProfessorProductController.cpp
  source/CustomStateMachine/CustomProductControllerState.cpp
  source/CustomStateMachine/CustomProductControllerStateBooting.cpp
  source/CustomStateMachine/CustomProductControllerStateIdle.cpp
  source/CustomStateMachine/CustomProductControllerStateIdleVoiceConfigured.cpp
  source/CustomStateMachine/CustomProductControllerStateIdleVoiceUnconfigured.cpp
  source/CustomStateMachine/CustomProductControllerStateNetworkStandbyConfigured.cpp
  source/CustomStateMachine/CustomProductControllerStateNetworkStandby.cpp
  source/CustomStateMachine/CustomProductControllerStateNetworkStandbyUnconfigured.cpp
  source/CustomStateMachine/CustomProductControllerStateOff.cpp
  source/CustomStateMachine/CustomProductControllerStateOn.cpp
  source/CustomStateMachine/CustomProductControllerStatePlayable.cpp
  source/CustomStateMachine/CustomProductControllerStatePlaying.cpp
  source/CustomStateMachine/CustomProductControllerStatePlayingActive.cpp
  source/CustomStateMachine/CustomProductControllerStatePlayingInactive.cpp
  source/CustomStateMachine/CustomProductControllerStateUpdatingSoftware.cpp
)

#
# lines duplicated on purpose, otherwise the linker complains
#
TARGET_LINK_LIBRARIES(Professor
  CastleProductControllerCommon
  ${SOUNDTOUCH_SDK_LIBS}
  ${SOUNDTOUCH_SDK_LIBS}
  LpmClient
  KeyHandler
  libjsoncpp.a
  libprotobuf.a
  libTinyXml.a
  pthread
  rt
)

INSTALL(TARGETS Professor DESTINATION ${OUTPUT_BIN_DIR})

# -----------------------------------------------------------------------------
