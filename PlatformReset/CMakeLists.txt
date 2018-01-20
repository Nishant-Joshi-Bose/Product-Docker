ADD_EXECUTABLE(PlatformReset
  Main.cpp
)

TARGET_LINK_LIBRARIES(PlatformReset
  LpmClient
  BoseLibs
  jsoncpp
  protobuf
  pthread
  rt
)

INSTALL(TARGETS PlatformReset DESTINATION ${OUTPUT_BIN_DIR})
