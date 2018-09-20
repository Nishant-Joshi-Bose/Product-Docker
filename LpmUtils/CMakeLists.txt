# ----------------------------------------------------------------------------

ADD_EXECUTABLE(PlatformReset
  PlatformReset.cpp
)

TARGET_LINK_LIBRARIES(PlatformReset
  LpmClient
  BoseLibs
  ProtoToMarkup
  jsoncpp
  protobuf
  pthread
  rt
)

INSTALL(TARGETS PlatformReset DESTINATION ${OUTPUT_BIN_DIR})

# ----------------------------------------------------------------------------

ADD_EXECUTABLE(PlatformResetIntoQfil
  PlatformResetIntoQfil.cpp
)

TARGET_LINK_LIBRARIES(PlatformResetIntoQfil
  LpmClient
  BoseLibs
  ProtoToMarkup
  jsoncpp
  protobuf
  pthread
  rt
)

INSTALL(TARGETS PlatformResetIntoQfil DESTINATION ${OUTPUT_BIN_DIR})

# ----------------------------------------------------------------------------

ADD_EXECUTABLE(ReadLpmVersion
  ReadLpmVersion.cpp
)

TARGET_LINK_LIBRARIES(ReadLpmVersion
  LpmClient
  BoseLibs
  ProtoToMarkup
  jsoncpp
  protobuf
  pthread
  rt
)

INSTALL(TARGETS ReadLpmVersion DESTINATION ${OUTPUT_BIN_DIR})

# ----------------------------------------------------------------------------
