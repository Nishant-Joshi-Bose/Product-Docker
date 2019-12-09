COMPONENT(JSONCPP_PATH opensource-jsoncpp-${SDK})
COMPONENT(HSP_PATH Riviera-Toolchain)
COMPONENT(OPENSSL_PATH opensource-openssl-${SDK})
COMPONENT(CASTLE_SEC_PATH SecureDeviceKey)

ADD_SUBDIRECTORY(${CASTLE_SEC_PATH} SecureDeviceKey)

ADD_EXECUTABLE(get-device-id
  get-device-id.cpp
)

TARGET_INCLUDE_DIRECTORIES(get-device-id PUBLIC
    ${OPENSSL_PATH}/include
    ${JSONCPP_PATH}/include
)

LINK_DIRECTORIES(
    ${OPENSSL_PATH}/lib
    ${JSONCPP_PATH}/lib
)

ADD_LIBRARY(jsoncpp STATIC IMPORTED)
set_property(TARGET jsoncpp PROPERTY IMPORTED_LOCATION ${JSONCPP_PATH}/lib/libjsoncpp.a)
ADD_LIBRARY(crypto STATIC IMPORTED)
set_property(TARGET crypto PROPERTY IMPORTED_LOCATION ${OPENSSL_PATH}/lib/libcrypto.a)
ADD_LIBRARY(ssl STATIC IMPORTED)
set_property(TARGET ssl PROPERTY IMPORTED_LOCATION ${OPENSSL_PATH}/lib/libssl.a)

TARGET_LINK_LIBRARIES(get-device-id
    CastleSecureDeviceKey
    jsoncpp
    ssl
    crypto
    dl
)

INSTALL(TARGETS get-device-id DESTINATION ${OUTPUT_BIN_DIR})
