cmake_minimum_required(VERSION 2.8.9)

project(lwm2mclient C)

option(DTLS "Enable DTLS" OFF)

include(${CMAKE_CURRENT_LIST_DIR}/../../core/wakaama.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/../../coap/coap.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/../../data/data.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/../shared/shared.cmake)
#include(${CMAKE_CURRENT_LIST_DIR}/../../http/http.cmake)

add_definitions(-DLWM2M_CLIENT_MODE)
add_definitions(-DLWM2M_BOOTSTRAP)
add_definitions(-DLWM2M_WITH_LOGS)
add_definitions(-DCG_USE_OPENSSL)
add_definitions(-DCOAP_PROTOCOL)
if(LWM2M_VERSION VERSION_GREATER "1.0")
    add_definitions(-DLWM2M_SUPPORT_SENML_JSON)
else()
    add_definitions(-DLWM2M_SUPPORT_JSON)
endif()

if(LWM2M_RAW_BLOCK1_REQUESTS)
    add_definitions(-DLWM2M_RAW_BLOCK1_REQUESTS)
endif()

include_directories (${WAKAAMA_HEADERS_DIR} ${COAP_HEADERS_DIR} ${DATA_HEADERS_DIR} ${WAKAAMA_SOURCES_DIR} ${SHARED_INCLUDE_DIRS})

SET(SOURCES
    ${CMAKE_CURRENT_LIST_DIR}/lwm2mclient.c
    ${CMAKE_CURRENT_LIST_DIR}/lwm2mclient.h
    ${CMAKE_CURRENT_LIST_DIR}/system_api.c
    ${CMAKE_CURRENT_LIST_DIR}/object_security.c
    ${CMAKE_CURRENT_LIST_DIR}/object_server.c
    ${CMAKE_CURRENT_LIST_DIR}/object_device.c
    ${CMAKE_CURRENT_LIST_DIR}/object_firmware.c
    ${CMAKE_CURRENT_LIST_DIR}/object_location.c
    ${CMAKE_CURRENT_LIST_DIR}/object_connectivity_moni.c
    ${CMAKE_CURRENT_LIST_DIR}/object_connectivity_stat.c
    ${CMAKE_CURRENT_LIST_DIR}/object_access_control.c
    ${CMAKE_CURRENT_LIST_DIR}/object_test.c
	${CMAKE_CURRENT_LIST_DIR}/object_cell_connectivity.c
	${CMAKE_CURRENT_LIST_DIR}/object_apn_conn_profile.c
	${CMAKE_CURRENT_LIST_DIR}/object_poweruplog.c
)

add_executable(${PROJECT_NAME} ${SOURCES} ${WAKAAMA_SOURCES} ${COAP_SOURCES} ${DATA_SOURCES} ${SHARED_SOURCES} ${HTTP_SOURCES})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${CMAKE_CURRENT_LIST_DIR}/../../../../../staging_dir/target-mipsel_r2_uClibc-0.9.33.2/usr/lib/libzcfg_msg.so ${CMAKE_CURRENT_LIST_DIR}/../../../../../staging_dir/target-mipsel_r2_uClibc-0.9.33.2/usr/lib/libzcmd_tool.so ${CMAKE_CURRENT_LIST_DIR}/../../../../../staging_dir/target-mipsel_r2_uClibc-0.9.33.2/usr/lib/libjson.so ${CMAKE_CURRENT_LIST_DIR}/../../../../../staging_dir/target-mipsel_r2_uClibc-0.9.33.2/usr/lib/libzcfg_fe_rdm_access.so ${CMAKE_CURRENT_LIST_DIR}/../../../../../staging_dir/target-mipsel_r2_uClibc-0.9.33.2/usr/lib/libzcfg_fe_rdm_struct.so ${CMAKE_CURRENT_LIST_DIR}/../../../../../staging_dir/target-mipsel_r2_uClibc-0.9.33.2/usr/lib/libzcfg_fe_rdm_string.so ${CMAKE_CURRENT_LIST_DIR}/../../../../../staging_dir/target-mipsel_r2_uClibc-0.9.33.2/usr/lib/libzyutil.so ${CMAKE_CURRENT_LIST_DIR}/../../../../../staging_dir/target-mipsel_r2_uClibc-0.9.33.2/usr/lib/libzcfg_fe_schema.so)

link_directories(${CMAKE_BINARY_DIR}/clinkc)
target_link_libraries(lwm2mclient clinkc)
target_link_libraries(lwm2mclient expat)
target_link_libraries(lwm2mclient uuid)
target_link_libraries(lwm2mclient ssl)
SOURCE_GROUP(wakaama FILES ${WAKAAMA_SOURCES})
install(TARGETS lwm2mclient DESTINATION ./)
