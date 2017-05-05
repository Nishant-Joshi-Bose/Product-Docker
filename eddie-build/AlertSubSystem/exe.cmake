cmake_minimum_required(VERSION 2.8)

add_compile_options("-std=gnu++11" "-g" "-Wformat=0" )

ADD_EXECUTABLE(AlertsSubsystem
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Alerts.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/AlertsScheduler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/AlertsSystemClient.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/AlertsManager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/AlertsMessage.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/AlsaAlertOutput.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/playAudio.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/mongoose.c
    )

TARGET_INCLUDE_DIRECTORIES(AlertsSubsystem PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    )

TARGET_LINK_LIBRARIES(AlertsSubsystem
	${SOUNDTOUCH_SDK_LIBS}
	-luuid 
	-lasound
    -lpthread
    -lrt
    )

INSTALL(TARGETS AlertsSubsystem DESTINATION ${OUTPUT_BIN_DIR})
