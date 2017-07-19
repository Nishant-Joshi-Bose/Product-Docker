ADD_EXECUTABLE(Eddie
  Main.cpp
  SystemClient/FrontDoorClient.cpp
)

TARGET_LINK_LIBRARIES(Eddie
  ${SOUNDTOUCH_SDK_LIBS}
  -lpthread
  -lrt
)

INSTALL(TARGETS Eddie DESTINATION ${OUTPUT_BIN_DIR})
