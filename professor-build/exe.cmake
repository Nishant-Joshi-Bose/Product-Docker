cmake_minimum_required(VERSION 2.8)

if(${SDK} STREQUAL "native")
    SET( SDK_INCLUDES
        ${SOUNDTOUCH_SDK_NATIVE}/prebuilt/include
        ${SOUNDTOUCH_SDK_NATIVE}/protobufs
        )
endif(${SDK} STREQUAL "native")

INCLUDE_DIRECTORIES(
    ${SDK_INCLUDES}
    ${HSP_INCLUDES}
    ${HSP_DRIVERS}
    )

#SET( DRIVER_SRCS
#    ${HSP_DRIVERS}/Device.cpp
#    ${HSP_DRIVERS}/DeviceBroker.cpp
#    ${HSP_DRIVERS}/DeviceCallbackListenerThread.cpp
#    ${HSP_DRIVERS}/GPIODeviceInput.cpp
#    ${HSP_DRIVERS}/KeyData.cpp
#    )

SET(EDDIE
    ${EDDIE_SOURCE_DIR}/Main.cpp
    #${EDDIE_SOURCE_DIR}/RhinoController/RhinoControllerTask.cpp
    #${EDDIE_SOURCE_DIR}/RhinoController/RhinoControllerITC.cpp
    #${EDDIE_SOURCE_DIR}/RhinoSoundTouchInterface/RhinoSoundTouchInterface.cpp
    )

SET(EDDIE_EXE Eddie)

ADD_EXECUTABLE(${EDDIE_EXE}
    ${EDDIE_SRCS}
    )

TARGET_LINK_LIBRARIES(${EDDIE_EXE}
        ${SOUNDTOUCH_SDK_TI}/prebuilt/Libs/libSoundTouchClient.a
        ${SOUNDTOUCH_SDK_TI}/prebuilt/Libs/libShelbyUtils.a
        -lpthread
        -lrt
        )

MESSAGE( STATUS "SDK: " ${SDK} )

#if(${SDK} STREQUAL "ti")
    #SET( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Os -g2 -DNDEBUG -std=gnu++11 -Wall -Wno-unused-but-set-variable -Wno-psabi -Werror -march=armv7-a -mtune=cortex-a8 -mfpu=neon -ftree-vectorize -mfloat-abi=hard -fno-omit-frame-pointer -mapcs-frame -fasynchronous-unwind-tables" )
#endif(${SDK} STREQUAL "ti")

#if(${SDK} STREQUAL "native")
    #SET( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Os -g2 -DNDEBUG -std=gnu++11 -Wall -Wno-unused-but-set-variable -Wno-psabi -Werror -ftree-vectorize -fno-omit-frame-pointer  -fasynchronous-unwind-tables" )
#endif(${SDK} STREQUAL "native")

# Install the targets

INSTALL(TARGETS ${EDDIE_EXE} DESTINATION ${OUTPUT_BIN_DIR})
