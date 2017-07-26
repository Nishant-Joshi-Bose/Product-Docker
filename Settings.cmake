EXECUTE_PROCESS(COMMAND components get SoundTouch-SDK-${SDK} installed_location
  OUTPUT_VARIABLE SOUNDTOUCH_SDK_DIR
  RESULT_VARIABLE RESULT
  OUTPUT_STRIP_TRAILING_WHITESPACE)
IF(RESULT)
  MESSAGE(FATAL_ERROR "No SOUNDTOUCH_SDK_DIR")
ENDIF()
MESSAGE(STATUS "SOUNDTOUCH_SDK_DIR is '${SOUNDTOUCH_SDK_DIR}'")

EXECUTE_PROCESS(COMMAND components get BoseLibs-${SDK} installed_location
  OUTPUT_VARIABLE BOSELIBS_DIR
  RESULT_VARIABLE RESULT
  OUTPUT_STRIP_TRAILING_WHITESPACE)
IF(RESULT)
  MESSAGE(FATAL_ERROR "No BOSELIBS_DIR")
ENDIF()
MESSAGE(STATUS "BOSELIBS_DIR is '${BOSELIBS_DIR}'")

IF(${SDK} STREQUAL "native")
  ADD_DEFINITIONS("-DENABLE_HOST_AUDIO_OUT -DNATIVE_BUILD")
  SET(CMAKE_INSTALL_RPATH "/opt/centos/glibc-2.15-toolchain/lib")
  SET(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
  SET(CMAKE_SKIP_BUILD_RPATH FALSE)
  MESSAGE(WARNING "Native builds don't work yet")
ENDIF(${SDK} STREQUAL "native")

IF(${SDK} STREQUAL "qc8017_32")
  EXECUTE_PROCESS(COMMAND components get Riviera-HSP installed_location
    OUTPUT_VARIABLE RIVIERA_HSP_DIR
    RESULT_VARIABLE RESULT
    OUTPUT_STRIP_TRAILING_WHITESPACE)
  IF(RESULT)
    MESSAGE(FATAL_ERROR "No RIVIERA_HSP_DIR")
  ENDIF()
  MESSAGE(STATUS "RIVIERA_HSP_DIR is '${RIVIERA_HSP_DIR}'")

  SET(SYSROOT ${RIVIERA_HSP_DIR}/sdk/sysroots/aarch64-oe-linux)

  SET(CMAKE_C_COMPILER ${RIVIERA_HSP_DIR}/sdk/sysroots/i686-oesdk-linux/usr/bin/arm-oemllib32-linux/arm-oemllib32-linux-gcc)
  SET(CMAKE_CXX_COMPILER ${RIVIERA_HSP_DIR}/sdk/sysroots/i686-oesdk-linux/usr/bin/arm-oemllib32-linux/arm-oemllib32-linux-g++)

  SET(COMMON_FLAGS "--sysroot=${SYSROOT} -Wall -Werror -Wno-psabi -Wno-maybe-uninitialized -Wno-unused-but-set-variable -mtune=cortex-a53 -ftree-vectorize -DPLATFORM_QC8017_32")
  SET(CMAKE_CXX_FLAGS "-std=c++1y ${COMMON_FLAGS}" CACHE INTERNAL - FORCE)
  SET(CMAKE_C_FLAGS "-std=gnu99 ${COMMON_FLAGS}" CACHE INTERNAL - FORCE)

  SET(CMAKE_INSTALL_RPATH "/opt/Bose/lib")
  SET(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
  SET(CMAKE_SKIP_BUILD_RPATH FALSE)

ENDIF(${SDK} STREQUAL "qc8017_32")

SET(CMAKE_INSTALL_PREFIX . CACHE INTERNAL - FORCE)
SET(OUTPUT_BIN_DIR bin)
SET(OUTPUT_LIB_DIR lib)

IF(${CFG} STREQUAL "Release")
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Os -g2 -DNDEBUG")
ENDIF(${CFG} STREQUAL "Release")

IF(${CFG} STREQUAL "Debug")
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g3 -fno-inline")
ENDIF(${CFG} STREQUAL "Debug")
