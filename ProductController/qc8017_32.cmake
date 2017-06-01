SET(COMMON_CXX_FLAGS "${EXTRA_FLAGS} -std=gnu++0x -std=c++1y -Wall -Wno-maybe-uninitialized -Wno-unused-but-set-variable -Wno-psabi -Werror -I${INC_PATH} -mtune=cortex-a53 -ftree-vectorize -DPLATFORM_QC8017_32")
SET(COMMON_C_FLAGS   "${EXTRA_FLAGS} -std=gnu99   -Wall -Wno-maybe-uninitialized -Wno-unused-but-set-variable -mtune=cortex-a53 -ftree-vectorize -DPLATFORM_QC8017_32")
SET(CMAKE_INSTALL_RPATH "/opt/Bose/lib")
SET(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
SET(CMAKE_SKIP_BUILD_RPATH FALSE)

SET(PLATFORM_COMPILER_FLAGS "--sysroot $ENV{CROSS_SYSROOT}")

MESSAGE(STATUS "LIB_PATH is ${LIB_PATH}")
FIND_LIBRARY(PROTOBUF_LIB    NAMES libprotobuf.so  PATHS  ${SOUNDTOUCH_SDK_DIR}/fs/opt/Bose/lib/ NO_DEFAULT_PATH)
MESSAGE(STATUS "PROTOBUF_LIB is ${PROTOBUF_LIB}")
