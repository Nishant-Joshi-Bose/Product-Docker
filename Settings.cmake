MACRO(COMPONENT VARIABLE_NAME COMPONENT_NAME)
  EXECUTE_PROCESS(COMMAND components get ${COMPONENT_NAME} installed_location
    OUTPUT_VARIABLE ${VARIABLE_NAME}
    RESULT_VARIABLE RESULT
    OUTPUT_STRIP_TRAILING_WHITESPACE)
  IF(RESULT)
    MESSAGE(FATAL_ERROR "No installed_location for component ${COMPONENT_NAME}")
  ENDIF()
  MESSAGE(STATUS "${VARIABLE_NAME} is '${${VARIABLE_NAME}}'")
ENDMACRO()

COMPONENT(A4VQUICKSET_DIR A4VQuickSetService-qc8017_32)
COMPONENT(A4VREMOTECOMMUNICATIONSERVICE_DIR A4VRemoteCommunicationService-qc8017_32)
COMPONENT(A4VVIDEOMANAGER_DIR A4VVideoManagerService-${SDK})
COMPONENT(AUDIOPATHCLIENT_DIR AudioPathClient-${SDK})
COMPONENT(AVSLIBS_DIR AVS-${SDK})
COMPONENT(BOSELIBS_DIR BoseLibs-${SDK})
COMPONENT(CLI_DIR Cli-${SDK})
COMPONENT(CASTLELIBS_DIR CastleLibs-${SDK})
COMPONENT(DATACOLLECTIONCLIENT_DIR DataCollectionClient-${SDK})
COMPONENT(HTTPINTERFACE_DIR HTTPInterface-${SDK})
COMPONENT(FRONTDOOR_DIR FrontDoor-${SDK})
COMPONENT(FRONTDOORSERVICE_DIR FrontDoorService-${SDK})
COMPONENT(FRONTDOORUTILITY_DIR FrontDoorUtility-${SDK})
COMPONENT(KEYHANDLER_DIR KeyHandler-${SDK})
COMPONENT(LIBWEBSOCKETS_DIR opensource-libwebsockets-${SDK})
COMPONENT(LIGHTBARCONTROLLER_DIR LightBarController-${SDK})
COMPONENT(NETWORKSERVICE_DIR NetworkService-${SDK})
COMPONENT(PASSPORTCLIENT_DIR PassportClient-${SDK})
COMPONENT(PRODUCTCONTROLLERCOMMON_DIR ProductControllerCommon)
COMPONENT(OPENSOURCEJSONCPP_DIR opensource-jsoncpp-${SDK})
COMPONENT(OPENSOURCEZLIB_DIR opensource-zlib-${SDK})
COMPONENT(PROTOBUF_DIR protobuf-${SDK})
COMPONENT(RIVIERALPMUPDATER_DIR RivieraLpmUpdater)
COMPONENT(RIVIERA_LPM_SERVICE_DIR RivieraLpmService-${SDK})
COMPONENT(SASS_DIR SASS-${SDK})
COMPONENT(SOUNDTOUCH_SDK_DIR SoundTouch-SDK-${SDK})
COMPONENT(UNITTEST_DIR unittest-cpp-${SDK})
COMPONENT(BLEREMOTEXCVR_DIR BLERemoteXCVR)
COMPONENT(SOFTWAREUPDATE_DIR SoftwareUpdate-${SDK})
COMPONENT(SYSTEMEVENTSERVICE_DIR SystemEventService-${SDK})
COMPONENT(GALAPAGOSCLIENT_DIR GalapagosClient-${SDK})
COMPONENT(AUDIOSOURCE_DIR AudioSource-${SDK})
COMPONENT(BTSTSSINK_DIR BTSTSSink-${SDK})
COMPONENT(BLUETOOTH_DIR Bluetooth-${SDK})
COMPONENT(BLESETUP_DIR BLESetup-${SDK})
COMPONENT(BLETOFRONTDOORSERVICE_DIR BLEToFrontDoorService-${SDK})
COMPONENT(PRODUCTCONTROLLERCOMMONPROTO_DIR ProductControllerCommonProto-${SDK})
COMPONENT(TELEMETRY_DIR Telemetry-${SDK})
COMPONENT(VPACONTROLLER_DIR VPAController-${SDK})

FIND_PROGRAM(CCACHE_EXE ccache)
IF(CCACHE_EXE AND USE_CCACHE)
  MESSAGE(STATUS "ccache is available and enabled, using it")
  SET_PROPERTY(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "${CCACHE_EXE}")
ELSE()
  MESSAGE(STATUS "ccache not found or not enabled (install ccache and set USE_CCACHE=1 to enable)")
ENDIF()

IF(${SDK} STREQUAL "native")
  ADD_DEFINITIONS("-DENABLE_HOST_AUDIO_OUT -DNATIVE_BUILD")
  SET(CMAKE_INSTALL_RPATH "/opt/centos/glibc-2.15-toolchain/lib")
  SET(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
  SET(CMAKE_SKIP_BUILD_RPATH FALSE)
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m32 -std=c++0x"
    CACHE INTERNAL - FORCE)
  SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -m32"
    CACHE INTERNAL - FORCE)
  EXECUTE_PROCESS(COMMAND getconf LONG_BIT
    OUTPUT_VARIABLE HOST_WORD_SIZE
    OUTPUT_STRIP_TRAILING_WHITESPACE)
  SET(TARGET_ARCH x86-${HOST_WORD_SIZE})
ENDIF(${SDK} STREQUAL "native")

IF(${SDK} STREQUAL "qc8017_32")
  COMPONENT(RIVIERA_HSP_IMAGES_DIR Riviera-HSP-Images)
  COMPONENT(RIVIERA_TOOLCHAIN_DIR Riviera-Toolchain)

  SET(SYSROOT ${RIVIERA_TOOLCHAIN_DIR}/sdk/sysroots/aarch64-oe-linux)

  EXECUTE_PROCESS(COMMAND uname -m
    OUTPUT_VARIABLE ARCHITECTURE
    OUTPUT_STRIP_TRAILING_WHITESPACE)
  MESSAGE(STATUS "Architecture is ${ARCHITECTURE}")

  SET(CMAKE_C_COMPILER ${RIVIERA_TOOLCHAIN_DIR}/sdk/sysroots/${ARCHITECTURE}-oesdk-linux/usr/bin/arm-oemllib32-linux/arm-oemllib32-linux-gcc
    CACHE INTERNAL - FORCE)
  SET(CMAKE_CXX_COMPILER ${RIVIERA_TOOLCHAIN_DIR}/sdk/sysroots/${ARCHITECTURE}-oesdk-linux/usr/bin/arm-oemllib32-linux/arm-oemllib32-linux-g++
    CACHE INTERNAL - FORCE)
  SET(CMAKE_OBJCOPY ${RIVIERA_TOOLCHAIN_DIR}/sdk/sysroots/${ARCHITECTURE}-oesdk-linux/usr/bin/arm-oemllib32-linux/arm-oemllib32-linux-objcopy
    CACHE INTERNAL - FORCE)

  SET(COMMON_FLAGS "--sysroot=${SYSROOT} -mtune=cortex-a53 -ftree-vectorize")
  SET(CMAKE_CXX_FLAGS "-std=c++11 ${COMMON_FLAGS}"
    CACHE INTERNAL - FORCE)
  SET(CMAKE_C_FLAGS "-std=gnu99 ${COMMON_FLAGS}"
    CACHE INTERNAL - FORCE)

  SET(CMAKE_INSTALL_RPATH "/opt/Bose/lib")
  SET(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
  SET(CMAKE_SKIP_BUILD_RPATH FALSE)

  SET(TARGET_ARCH arm-oemllib32-linux)
ENDIF(${SDK} STREQUAL "qc8017_32")

SET(PROTO_CC_DEST "${CMAKE_BINARY_DIR}/proto")
SET(CMAKE_INSTALL_PREFIX . CACHE INTERNAL - FORCE)
SET(OUTPUT_BIN_DIR bin)
SET(OUTPUT_LIB_DIR lib)
SET(CUSTOM_HSM_DIR "${CMAKE_SOURCE_DIR}/ProductController/CustomHsm")
SET(INTENT_HANDLER_DIR "${CMAKE_SOURCE_DIR}/ProductController/IntentHandler")
SET(CUSTOM_HARDWARE_DIR "${CMAKE_SOURCE_DIR}/ProductController/CustomProductLpmHardwareInterface")
SET(CUSTOM_PRODUCT_AUDIOSERVICE_DIR "${CMAKE_SOURCE_DIR}/ProductController/CustomProductAudioService")

IF(${CFG} STREQUAL "Release")
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Os -g2 -DNDEBUG")
ENDIF(${CFG} STREQUAL "Release")

IF(${CFG} STREQUAL "Debug")
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g3 -fno-inline")
ENDIF(${CFG} STREQUAL "Debug")

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -Woverloaded-virtual -Wnon-virtual-dtor"
    CACHE INTERNAL - FORCE)
SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Werror"
    CACHE INTERNAL - FORCE)

INCLUDE_DIRECTORIES(
  ${CMAKE_BINARY_DIR}
  ${PROTO_CC_DEST}
  ${AUDIOPATHCLIENT_DIR}/include
  ${CMAKE_SOURCE_DIR}/builds/RivieraLPM
  ${RIVIERA_LPM_SERVICE_DIR}/include
  ${BOSELIBS_DIR}/include
  ${BOSELIBS_DIR}/protobufs
  ${PROTOBUF_DIR}/include
  ${OPENSOURCEZLIB_DIR}/include
  ${CASTLELIBS_DIR}/include
  ${CLI_DIR}/include
  ${DATACOLLECTIONCLIENT_DIR}/include
  ${HTTPINTERFACE_DIR}/include
  ${LIGHTBARCONTROLLER_DIR}/include
  ${NETWORKSERVICE_DIR}/include
  ${PASSPORTCLIENT_DIR}/include
  ${AUDIOSOURCE_DIR}/include
  ${SOUNDTOUCH_SDK_DIR}/prebuilt/include
  ${SOUNDTOUCH_SDK_DIR}/prebuilt/include/Bluetooth
  ${AVSLIBS_DIR}/include
  ${LIBWEBSOCKETS_DIR}/include
  ${FRONTDOOR_DIR}/include
  ${FRONTDOOR_DIR}/include/FrontDoor
  ${SASS_DIR}/include
  ${UNITTEST_DIR}/include
  ${SOFTWAREUPDATE_DIR}/include/API
  ${SYSTEMEVENTSERVICE_DIR}/include
  ${GALAPAGOSCLIENT_DIR}/include
  ${OPENSOURCEJSONCPP_DIR}/include
  ${BTSTSSINK_DIR}/include
  ${BLESETUP_DIR}/include
  ${BLUETOOTH_DIR}/include
  ${PRODUCTCONTROLLERCOMMONPROTO_DIR}/include
  ${A4VREMOTECOMMUNICATIONSERVICE_DIR}/include
  ${A4VVIDEOMANAGER_DIR}/include
  ${A4VQUICKSET_DIR}/include
  ${VPACONTROLLER_DIR}/include/VoiceServiceClient
  ${VPACONTROLLER_DIR}/include/VPAControllerClient
)

LINK_DIRECTORIES(
  ${AUDIOPATHCLIENT_DIR}/lib
  ${BOSELIBS_DIR}/lib
  ${CLI_DIR}/lib
  ${CASTLELIBS_DIR}/lib
  ${PASSPORTCLIENT_DIR}/lib
  ${PROTOBUF_DIR}/lib
  ${PRODUCTCONTROLLERCOMMONPROTO_DIR}/lib
  ${DATACOLLECTIONCLIENT_DIR}/lib
  ${HTTPINTERFACE_DIR}/lib
  ${RIVIERA_LPM_SERVICE_DIR}/lib
  ${LIGHTBARCONTROLLER_DIR}/lib
  ${NETWORKSERVICE_DIR}/lib
  ${AUDIOSOURCE_DIR}/lib
  ${SOUNDTOUCH_SDK_DIR}/prebuilt/Libs
  ${KEYHANDLER_DIR}/lib
  ${AVSLIBS_DIR}/lib
  ${FRONTDOOR_DIR}/lib
  ${SASS_DIR}/lib
  ${SOFTWAREUPDATE_DIR}/lib
  ${SYSTEMEVENTSERVICE_DIR}/lib
  ${GALAPAGOSCLIENT_DIR}/lib
  ${OPENSOURCEJSONCPP_DIR}/lib
  ${BTSTSSINK_DIR}/lib
  ${BLESETUP_DIR}/lib
  ${BLUETOOTH_DIR}/lib
  ${LIBWEBSOCKETS_DIR}/lib
  ${OPENSOURCEZLIB_DIR}/lib
  ${A4VVIDEOMANAGER_DIR}/lib
  ${A4VREMOTECOMMUNICATIONSERVICE_DIR}/lib
  ${A4VQUICKSET_DIR}/lib
  ${VPACONTROLLER_DIR}/lib
)
