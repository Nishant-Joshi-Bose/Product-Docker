##
## flags.cmake - compiler and linker flags for CMAKE builds
##

SET(COMMON_CXX_FLAGS "${EXTRA_FLAGS} -std=gnu++0x -Wall -Wno-unused-but-set-variable -m32 -Wno-psabi -I${INC_PATH} -DPLATFORM_NATIVE")
SET(COMMON_C_FLAGS   "${EXTRA_FLAGS} -std=gnu99   -Wall -Wno-unused-but-set-variable -m32 -DPLATFORM_NATIVE")

ADD_DEFINITIONS("-DENABLE_HOST_AUDIO_OUT -DNATIVE_BUILD")
SET(DISABLE_UNUSED_WARNING_FLAGS "-Wno-unused")
IF( ${GLIB_215_NEEDED} )
  # these three CMAKE settings are the equivalent of "-Wl,--rpath=/opt/centos/glibc-2.15-toolchain/lib":
  SET(CMAKE_INSTALL_RPATH "/opt/centos/glibc-2.15-toolchain/lib")
  SET(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
  SET(CMAKE_SKIP_BUILD_RPATH FALSE)
ENDIF( ${GLIB_215_NEEDED} )

# TODO update this to point to the STSDK protobuf lib file once there is a native sdk tarball
FIND_LIBRARY(PROTOBUF_LIB    NAMES libprotobuf.so  PATHS  ${LIB_PATH} NO_DEFAULT_PATH)
