SET( LIB_NAME RepeatManagerLib )

SET( LIB_SRCS
RepeatManagerController.cpp
RepeatManagerCliClient.cpp
RepeatManager.cpp
RepeatManager.pb.cc
RepeatManagerKeyAction.pb.cc
)

ADD_LIBRARY(${LIB_NAME} STATIC
${LIB_SRCS}
)

INSTALL(TARGETS ${LIB_NAME} DESTINATION ${OUTPUT_LIB_DIR})
