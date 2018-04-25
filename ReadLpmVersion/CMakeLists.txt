ADD_EXECUTABLE(ReadLpmVersion
  ReadLpmVersion.cpp
)

TARGET_LINK_LIBRARIES(ReadLpmVersion
  LpmClient
  BoseLibs
  jsoncpp
  protobuf
  pthread
  rt
)

INSTALL(TARGETS ReadLpmVersion DESTINATION ${OUTPUT_BIN_DIR})
