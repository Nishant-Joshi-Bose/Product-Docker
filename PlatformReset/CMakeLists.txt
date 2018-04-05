# ----------------------------------------------------------------------------

ADD_EXECUTABLE(PlatformReset
  PlatformReset.cpp
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

# ----------------------------------------------------------------------------

ADD_EXECUTABLE(PlatformResetIntoQfil
  PlatformResetIntoQfil.cpp
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

# ----------------------------------------------------------------------------
