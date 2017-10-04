MESSAGE( STATUS "ProductController CMakeLists.txt CUSTOM_HSM_DIR                       : " ${CUSTOM_HSM_DIR} )
MESSAGE( STATUS "ProductController CMakeLists.txt PROTO_CC_DEST                        : " ${PROTO_CC_DEST} )
MESSAGE( STATUS "ProductController CMakeLists.txt NETWORK_COMMON_LIBS                  : " ${NETWORK_COMMON_LIBS} )
MESSAGE( STATUS "ProductController CMakeLists.txt NETWORK_COMMON_INCLUDE               : " ${NETWORK_COMMON_INCLUDE} )
MESSAGE( STATUS "ProductController CMakeLists.txt SOUNDTOUCH_SDK_DIR                   : " ${SOUNDTOUCH_SDK_DIR} )
MESSAGE( STATUS "ProductController CMakeLists.txt RIVIERA_LPM_DIR                      : " ${RIVIERA_LPM_DIR} )
MESSAGE( STATUS "ProductController CMakeLists.txt RIVIERA_LPM_SERVICE_DIR              : " ${RIVIERA_LPM_SERVICE_DIR} )
MESSAGE( STATUS "ProductController CMakeLists.txt OUTPUT_BIN_DIR                       : " ${OUTPUT_BIN_DIR} )
MESSAGE( STATUS "ProductController CMakeLists.txt CASTLE_PRODUCT_CONTROLLER_COMMON_DIR : " ${CASTLE_PRODUCT_CONTROLLER_COMMON_DIR} )

INCLUDE_DIRECTORIES(
  ${NETWORK_COMMON_INCLUDE}
  ${SOUNDTOUCH_SDK_DIR}/prebuilt/include/SoundTouchInterface
  ${RIVIERA_LPM_DIR}/include
  ${RIVIERA_LPM_SERVICE_DIR}/source/Client
  ${RIVIERA_LPM_SERVICE_DIR}/source/LPMIPC
  ${RIVIERA_LPM_DIR}/include
  ${CASTLE_PRODUCT_CONTROLLER_COMMON_DIR}
  ${CASTLE_PRODUCT_CONTROLLER_COMMON_DIR}/Hsm
  ${KEYHANDLER_DIR}/include
  .
  ${CASTLE_PRODUCT_CONTROLLER_COMMON_DIR}/ProductAudioService 
  include
  source
  source/CustomStateMachine
  ../ProductSTSController
)

SET(SOUNDTOUCH_SDK_LIBS
  SoundTouchClient
  FrontDoorClient
  ShelbyUtils
  BoseLibs
  ${NETWORK_COMMON_LIBS}/libNetworkServiceUtils.a
  ${NETWORK_COMMON_LIBS}/libNetworkServiceClient.a
  STS
)

# -----------------------------------------------------------------------------
# Professor daemon.

ADD_EXECUTABLE(Professor
  ${PROTO_CC_DEST}/A4VPersistence.pb.cc
  ${PROTO_CC_DEST}/A4VSystemTimeout.pb.cc
  ${PROTO_CC_DEST}/AudioSettings.pb.cc
  ${PROTO_CC_DEST}/ProductMessage.pb.cc
  ${PROTO_CC_DEST}/RebroadcastLatencyMode.pb.cc
  ${PROTO_CC_DEST}/ConfigurationStatus.pb.cc
  ${PROTO_CC_DEST}/Language.pb.cc
  ${PROTO_CC_DEST}/ContentItem.pb.cc
  source/ProductStart.cpp
  source/ProductCommandLine.cpp
  source/ProductHardwareInterface.cpp
  source/ProductSoftwareServices.cpp
  source/ProductFrontDoorNetwork.cpp
  source/ProductUserInterface.cpp
  source/ProfessorProductController.cpp
  source/CustomStateMachine/CustomProductControllerStateBooting.cpp
  source/CustomStateMachine/CustomProductControllerState.cpp
  source/CustomStateMachine/CustomProductControllerStateIdle.cpp
  source/CustomStateMachine/CustomProductControllerStateNetworkStandby.cpp
  source/CustomStateMachine/CustomProductControllerStateUpdating.cpp
  ../ProductSTSController/ProductSTSStateTop.cpp
  ../ProductSTSController/ProductSTSController.cpp
  ../ProductSTSController/ProductSTSAccount.cpp
  ${PROTO_CC_DEST}/ProductSTSIntrospect.pb.cc
  ${PROTO_CC_DEST}/ProductSTS.pb.cc
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
  jsoncpp
  protobuf
  -lpthread
  -lrt
  -ltinyxml
)

INSTALL(TARGETS Professor DESTINATION ${OUTPUT_BIN_DIR})

# -----------------------------------------------------------------------------
