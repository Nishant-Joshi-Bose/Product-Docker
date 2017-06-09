cmake_minimum_required(VERSION 2.8)

SET(EDDIE_SRCS
    ${EDDIE_SOURCE_DIR}/Main.cpp
    ${EDDIE_SOURCE_DIR}/SystemClient/FrontDoorClient.cpp
    #${EDDIE_SOURCE_DIR}/EddieController/EddieControllerTask.cpp
    #${EDDIE_SOURCE_DIR}/EddieController/EddieControllerITC.cpp
    #${EDDIE_SOURCE_DIR}/EddieSoundTouchInterface/EddieSoundTouchInterface.cpp
    )

SET(EDDIE_EXE Eddie)

ADD_EXECUTABLE(${EDDIE_EXE}
    ${EDDIE_SRCS}
    )

TARGET_LINK_LIBRARIES(${EDDIE_EXE}
	${SOUNDTOUCH_SDK_LIBS}
        -lpthread
        -lrt
        )

MESSAGE( STATUS "SDK: " ${SDK} )

INSTALL(TARGETS ${EDDIE_EXE} DESTINATION ${OUTPUT_BIN_DIR})
