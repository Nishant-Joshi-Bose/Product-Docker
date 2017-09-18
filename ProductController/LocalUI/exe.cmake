ADD_EXECUTABLE(CapSenseWebServer
  capsense_web_server.c
  mongoose.c
)

TARGET_LINK_LIBRARIES(CapSenseWebServer
  pthread
)

INSTALL(TARGETS CapSenseWebServer DESTINATION ${OUTPUT_BIN_DIR})
