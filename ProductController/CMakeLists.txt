INCLUDE_DIRECTORIES(
  ${PRODUCTCONTROLLERCOMMON_DIR}
  ${PRODUCTCONTROLLERCOMMON_DIR}/ChimesController
  ${PRODUCTCONTROLLERCOMMON_DIR}/Hsm
  ${PRODUCTCONTROLLERCOMMON_DIR}/ProductSTSController
  ${PRODUCTCONTROLLERCOMMON_DIR}/ProductAudioService
  ${PRODUCTCONTROLLERCOMMON_DIR}/ProductLpmHardwareInterface
  ${PRODUCTCONTROLLERCOMMON_DIR}/IntentHandler
  ${DATACOLLECTIONCLIENT_DIR}
  ${CUSTOM_HSM_DIR}
  ${KEYHANDLER_DIR}/include
  IntentHandler
  CustomProductAudioService
  DisplayController
  .
)

# -----------------------------------------------------------------------------
# Eddie daemon.

ADD_EXECUTABLE(Eddie
  ${CUSTOM_HSM_DIR}/CustomProductControllerState.cpp
  ${CUSTOM_HSM_DIR}/CustomProductControllerStateBooting.cpp
  ${CUSTOM_HSM_DIR}/CustomProductControllerStateSetup.cpp
  ${CUSTOM_HSM_DIR}/CustomProductControllerStateOn.cpp
  ProductCliClient.cpp
  DataCollectionClientInterface.cpp
  EddieProductController.cpp
  Main.cpp
  ${PROTO_CC_DEST}/ConfigurationStatus.pb.cc
  ${PROTO_CC_DEST}/ProductMessage.pb.cc
  ${PROTO_CC_DEST}/KeyActions.pb.cc
  ${PROTO_CC_DEST}/DisplayController.pb.cc
  ${PROTO_CC_DEST}/IntentHandler.pb.cc
  ${PROTO_CC_DEST}/AudioSettings.pb.cc
  ${PROTO_CC_DEST}/ProductSetupState.pb.cc
  ${PROTO_CC_DEST}/VolumeConfig.pb.cc
  IntentHandler/CountDownManager.cpp
  IntentHandler/IntentHandler.cpp
  IntentHandler/PlaybackRequestManager.cpp
  DisplayController/DisplayController.cpp
  CustomProductAudioService/CustomProductAudioService.cpp
  CustomProductAudioService/CustomAudioSettingsManager.cpp
  CustomProductLpmHardwareInterface.cpp
)

TARGET_LINK_LIBRARIES(Eddie
  CastleProductControllerCommon
  DataCollectionClient
  LpmClient
  HttpInterface
  FrontDoorClient
  NetworkServiceClient
  KeyHandler
  BLESetupServiceProto
  BluetoothSinkServiceProto
  LightBarController
  VoiceServiceClient
  CliClient
  AudioLibs
  PassportClient
  #SASSFrontDoorServiceProto # TODO: SASS integration
  SoftwareUpdateAPI
  STS
  ShelbyUtils
  SoundTouch_SDK_Protobuf_static
  BoseLibs
  jsoncpp
  protobuf
  TinyXml
  pthread
  rt
  cap
  dl
)

INSTALL(TARGETS Eddie DESTINATION ${OUTPUT_BIN_DIR})
