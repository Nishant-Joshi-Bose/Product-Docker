cmake_minimum_required(VERSION 2.8)

SET(PROFESSOR_SRCS
    ${PROFESSOR_SOURCE_DIR}/Main.cpp
    #${PROFESSOR_SOURCE_DIR}/EddieController/EddieControllerTask.cpp
    #${PROFESSOR_SOURCE_DIR}/EddieController/EddieControllerITC.cpp
    #${PROFESSOR_SOURCE_DIR}/EddieSoundTouchInterface/EddieSoundTouchInterface.cpp
    )

SET(PROFESSOR_EXE Eddie)

ADD_EXECUTABLE(${PROFESSOR_EXE}
    ${PROFESSOR_SRCS}
    )

TARGET_LINK_LIBRARIES(${PROFESSOR_EXE}
	${SOUNDTOUCH_SDK_LIBS}
        -lpthread
        -lrt
        )

MESSAGE( STATUS "SDK: " ${SDK} )

INSTALL(TARGETS ${PROFESSOR_EXE} DESTINATION ${OUTPUT_BIN_DIR})
