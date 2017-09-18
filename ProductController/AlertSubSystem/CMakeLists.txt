ADD_EXECUTABLE(AlertsSubsystem
  main.cpp
  Alerts.cpp
  AlertsScheduler.cpp
  AlertsSystemClient.cpp
  AlertsManager.cpp
  AlertsMessage.cpp
  AlsaAlertOutput.cpp
  playAudio.cpp
  mongoose.c
)

TARGET_INCLUDE_DIRECTORIES(AlertsSubsystem PUBLIC
  include
)

TARGET_LINK_LIBRARIES(AlertsSubsystem
  ${SOUNDTOUCH_SDK_LIBS}
  -luuid 
  -lasound
  -lpthread
  -lrt
)

INSTALL(TARGETS AlertsSubsystem DESTINATION ${OUTPUT_BIN_DIR})
