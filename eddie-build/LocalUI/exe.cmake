cmake_minimum_required(VERSION 2.8)

ADD_EXECUTABLE(CapSenseWebServer
    ${CMAKE_CURRENT_SOURCE_DIR}/capsense_web_server.c
    ${CMAKE_CURRENT_SOURCE_DIR}/mongoose.c
    )

TARGET_LINK_LIBRARIES(CapSenseWebServer
#	${SOUNDTOUCH_SDK_LIBS}
    -lpthread
    -lrt
    )

INSTALL(TARGETS CapSenseWebServer DESTINATION ${OUTPUT_BIN_DIR})
