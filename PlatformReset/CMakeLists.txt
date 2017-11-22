ADD_EXECUTABLE(PlatformReset
  Main.cpp
)

TARGET_LINK_LIBRARIES(PlatformReset
  BoseLibs
  pthread
)

INSTALL(TARGETS PlatformReset DESTINATION ${OUTPUT_BIN_DIR})
