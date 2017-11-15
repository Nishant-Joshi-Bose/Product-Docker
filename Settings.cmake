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

COMPONENT(BOSELIBS_DIR BoseLibs-${SDK})
COMPONENT(CASTLECLI_DIR CastleCli-${SDK})
COMPONENT(CASTLELIBS_DIR CastleLibs-${SDK})
COMPONENT(DEMOCONTROLLER_DIR CastleDemoController-${SDK})
COMPONENT(KEYHANDLER_DIR KeyHandler-${SDK})
COMPONENT(LIGHTBARCONTROLLER_DIR CastleLightBarController-${SDK})
COMPONENT(NETWORKSERVICE_DIR CastleNetworkService-${SDK})
COMPONENT(PASSPORTCLIENT_DIR CastlePassportClient-${SDK})
COMPONENT(PRODUCTCONTROLLERCOMMON_DIR CastleProductControllerCommon)
COMPONENT(PROTOBUF_DIR protobuf-${SDK})
COMPONENT(RIVIERA_LPM_SERVICE_DIR RivieraLpmService-${SDK})
COMPONENT(SOUNDTOUCH_SDK_DIR SoundTouch-SDK-${SDK})
COMPONENT(UNITTEST_DIR UnitTest++-${SDK})

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

  SET(COMMON_FLAGS "--sysroot=${SYSROOT} -mtune=cortex-a53 -ftree-vectorize")
  SET(CMAKE_CXX_FLAGS "-std=c++11 ${COMMON_FLAGS}"
    CACHE INTERNAL - FORCE)
  SET(CMAKE_C_FLAGS "-std=gnu99 ${COMMON_FLAGS}"
    CACHE INTERNAL - FORCE)

  SET(CMAKE_INSTALL_RPATH "/opt/Bose/lib")
  SET(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
  SET(CMAKE_SKIP_BUILD_RPATH FALSE)

ENDIF(${SDK} STREQUAL "qc8017_32")

SET(PROTO_CC_DEST "${CMAKE_BINARY_DIR}/proto")
SET(CMAKE_INSTALL_PREFIX . CACHE INTERNAL - FORCE)
SET(OUTPUT_BIN_DIR bin)
SET(OUTPUT_LIB_DIR lib)
SET(CUSTOM_HSM_DIR "${CMAKE_SOURCE_DIR}/ProductController/CustomHsm")

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
  ${CMAKE_SOURCE_DIR}/builds/RivieraLPM
  ${RIVIERA_LPM_SERVICE_DIR}/include
  ${BOSELIBS_DIR}/include
  ${BOSELIBS_DIR}/protobufs
  ${PROTOBUF_DIR}/include
  ${CASTLELIBS_DIR}/include
  ${CASTLECLI_DIR}/include
  ${DEMOCONTROLLER_DIR}/include
  ${LIGHTBARCONTROLLER_DIR}/include
  ${NETWORKSERVICE_DIR}/include
  ${PASSPORTCLIENT_DIR}/include
  ${SOUNDTOUCH_SDK_DIR}/prebuilt/include
  ${SOUNDTOUCH_SDK_DIR}/prebuilt/include/Bluetooth
  ${UNITTEST_DIR}/include
)

LINK_DIRECTORIES(
  ${BOSELIBS_DIR}/lib
  ${CASTLECLI_DIR}/lib
  ${DEMOCONTROLLER_DIR}/lib
  ${CASTLELIBS_DIR}/lib
  ${PASSPORTCLIENT_DIR}/lib
  ${PROTOBUF_DIR}/lib
  ${RIVIERA_LPM_SERVICE_DIR}/lib
  ${LIGHTBARCONTROLLER_DIR}/lib
  ${NETWORKSERVICE_DIR}/lib
  ${SOUNDTOUCH_SDK_DIR}/prebuilt/Libs
  ${KEYHANDLER_DIR}/lib
)
