MESSAGE( STATUS "ProductController CMakeLists.txt CUSTOM_HSM_DIR     : " ${CUSTOM_HSM_DIR} )
MESSAGE( STATUS "ProductController CMakeLists.txt PROTO_CC_DEST      : " ${PROTO_CC_DEST} )
MESSAGE( STATUS "ProductController CMakeLists.txt NETWORK_COMMON_LIBS: " ${NETWORK_COMMON_LIBS} )

INCLUDE_DIRECTORIES(
  ${NETWORK_COMMON_INCLUDE}
  ${SOUNDTOUCH_SDK_DIR}/prebuilt/include/SoundTouchInterface
  ${RIVIERA_LPM_DIR}/include
  ${RIVIERA_LPM_SERVICE_DIR}/source/Client
  ${RIVIERA_LPM_SERVICE_DIR}/source/LPMIPC
  ${RIVIERA_LPM_DIR}/include
  ${CASTLE_PRODUCT_CONTROLLER_COMMON_DIR}
  ${CASTLE_PRODUCT_CONTROLLER_COMMON_DIR}/Hsm
  include
  source
  source/CustomStateMachine
)

SET(SOUNDTOUCH_SDK_LIBS
  ${SOUNDTOUCH_SDK_DIR}/prebuilt/Libs/libSoundTouchClient.a
  ${SOUNDTOUCH_SDK_DIR}/prebuilt/Libs/libFrontDoorClient.a
  ${SOUNDTOUCH_SDK_DIR}/prebuilt/Libs/libShelbyUtils.a
  ${BOSELIBS_DIR}/lib/libBoseLibs.a
  ${NETWORK_COMMON_LIBS}/libNetworkServiceUtils.a
  ${NETWORK_COMMON_LIBS}/libNetworkServiceClient.a
)

# -----------------------------------------------------------------------------
# Professor daemon.

ADD_EXECUTABLE(Professor
  ${PROTO_CC_DEST}/A4VPersistence.pb.cc
  ${PROTO_CC_DEST}/A4VSystemTimeout.pb.cc
  ${PROTO_CC_DEST}/AudioControls.pb.cc
  ${PROTO_CC_DEST}/ProductMessage.pb.cc
  ${PROTO_CC_DEST}/RebroadcastLatencyMode.pb.cc
  ${PROTO_CC_DEST}/ConfigurationStatus.pb.cc
  ${PROTO_CC_DEST}/Language.pb.cc
  source/ProductStart.cpp
  source/ProductCommandLine.cpp
  source/ProductHardwareInterface.cpp
  source/ProductAudioServices.cpp
  source/ProductDeviceSettings.cpp
  source/ProductSoftwareServices.cpp
  source/ProductFrontDoorNetwork.cpp
  source/ProductUserInterface.cpp
  source/ProfessorProductController.cpp
  source/CustomStateMachine/CustomProductControllerStateBooting.cpp
  source/CustomStateMachine/CustomProductControllerState.cpp
  source/CustomStateMachine/CustomProductControllerStateIdle.cpp
  source/CustomStateMachine/CustomProductControllerStateNetworkStandby.cpp
  source/CustomStateMachine/CustomProductControllerStateUpdating.cpp
)

TARGET_LINK_LIBRARIES(Professor
  CastleProductControllerCommon
  ${SOUNDTOUCH_SDK_LIBS}
  LpmClient
  jsoncpp
  protobuf
  -lpthread
  -lrt
  -ltinyxml
)

INSTALL(TARGETS Professor DESTINATION ${OUTPUT_BIN_DIR})

# -----------------------------------------------------------------------------
