ADD_EXECUTABLE(PlatformResetIntoQfil
  Main.cpp
)

TARGET_LINK_LIBRARIES(PlatformResetIntoQfil
  LpmClient
  BoseLibs
  jsoncpp
  protobuf
  pthread
  rt
)

INSTALL(TARGETS PlatformResetIntoQfil DESTINATION ${OUTPUT_BIN_DIR})
