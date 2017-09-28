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
COMPONENT(CASTLELIBS_DIR CastleLibs-${SDK})
COMPONENT(PROTOBUF_DIR protobuf-${SDK})
COMPONENT(SOUNDTOUCH_SDK_DIR SoundTouch-SDK-${SDK})
COMPONENT(RIVIERA_LPM_DIR RivieraLPM)
COMPONENT(RIVIERA_LPM_SERVICE_DIR RivieraLpmService)
COMPONENT(CASTLE_PRODUCT_CONTROLLER_COMMON_DIR CastleProductControllerCommon)
COMPONENT(GOOGLETEST_DIR googletest-${SDK})
COMPONENT(NETWORK_SERVICE_DIR CastleNetworkService-${SDK})

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
  COMPONENT(RIVIERA_HSP_DIR Riviera-HSP)

  SET(SYSROOT ${RIVIERA_HSP_DIR}/sdk/sysroots/aarch64-oe-linux)

  EXECUTE_PROCESS(COMMAND uname -m
    OUTPUT_VARIABLE ARCHITECTURE
    OUTPUT_STRIP_TRAILING_WHITESPACE)
  MESSAGE(STATUS "Architecture is ${ARCHITECTURE}")

  SET(CMAKE_C_COMPILER ${RIVIERA_HSP_DIR}/sdk/sysroots/${ARCHITECTURE}-oesdk-linux/usr/bin/arm-oemllib32-linux/arm-oemllib32-linux-gcc
    CACHE INTERNAL - FORCE)
  SET(CMAKE_CXX_COMPILER ${RIVIERA_HSP_DIR}/sdk/sysroots/${ARCHITECTURE}-oesdk-linux/usr/bin/arm-oemllib32-linux/arm-oemllib32-linux-g++
    CACHE INTERNAL - FORCE)

  SET(COMMON_FLAGS "--sysroot=${SYSROOT} -Wall -Werror -mtune=cortex-a53 -ftree-vectorize")
  SET(CMAKE_CXX_FLAGS "-std=c++11 ${COMMON_FLAGS}"
    CACHE INTERNAL - FORCE)
  SET(CMAKE_C_FLAGS "-std=gnu99 ${COMMON_FLAGS}"
    CACHE INTERNAL - FORCE)

  SET(CMAKE_INSTALL_RPATH "/opt/Bose/lib")
  SET(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
  SET(CMAKE_SKIP_BUILD_RPATH FALSE)

ENDIF(${SDK} STREQUAL "qc8017_32")

SET(PRODUCT_STATE_COMMON_SOURCE  "${CASTLE_PRODUCT_CONTROLLER_COMMON_DIR}")
SET(PRODUCT_STATE_COMMON_INCLUDE "${CASTLE_PRODUCT_CONTROLLER_COMMON_DIR}")
SET(LPM_STATE_COMMON_SOURCE      "${RIVIERA_LPM_SERVICE_DIR}")
SET(LPM_STATE_COMMON_INCLUDE     "${RIVIERA_LPM_SERVICE_DIR}/source/Client")
SET(NETWORK_COMMON_INCLUDE       "${NETWORK_SERVICE_DIR}/include")
SET(NETWORK_COMMON_LIBS          "${NETWORK_SERVICE_DIR}/lib")

SET(PROTO_CC_DEST "${CMAKE_BINARY_DIR}/proto")
SET(CMAKE_INSTALL_PREFIX . CACHE INTERNAL - FORCE)
SET(OUTPUT_BIN_DIR bin)
SET(OUTPUT_LIB_DIR lib)

#Custom Hsm directory path needs set before building CastleProductControllerCommon library.
SET(CUSTOM_HSM_DIR ${CMAKE_SOURCE_DIR}/ProductController/source/CustomStateMachine)

MESSAGE( STATUS "Settings.cmake NETWORK_COMMON_LIBS: " ${NETWORK_COMMON_LIBS} )

IF(${CFG} STREQUAL "Release")
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Os -g2 -DNDEBUG")
ENDIF(${CFG} STREQUAL "Release")

IF(${CFG} STREQUAL "Debug")
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g3 -fno-inline")
ENDIF(${CFG} STREQUAL "Debug")

INCLUDE_DIRECTORIES(
  ${CMAKE_BINARY_DIR}
  ${PROTO_CC_DEST}
  ${BOSELIBS_DIR}/include
  ${BOSELIBS_DIR}/protobufs
  ${PROTOBUF_DIR}/include
  ${CASTLELIBS_DIR}/include
  ${SOUNDTOUCH_SDK_DIR}/prebuilt/include
  ${SOUNDTOUCH_SDK_DIR}/prebuilt/include/STS
)

LINK_DIRECTORIES(
  ${BOSELIBS_DIR}/lib
  ${CASTLELIBS_DIR}/lib
  ${PROTOBUF_DIR}/lib
  ${SOUNDTOUCH_SDK_DIR}/prebuilt/Libs
)
