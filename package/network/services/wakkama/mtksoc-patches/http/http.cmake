# Provides HTTP_SOURCES_DIR and HTTP_HEADERS_DIR variables.
# Add LWM2M_WITH_LOGS to compile definitions to enable logging.

set(HTTP_SOURCES_DIR ${CMAKE_CURRENT_LIST_DIR})
set(HTTP_HEADERS_DIR ${CMAKE_CURRENT_LIST_DIR})

set(HTTP_SOURCES
    ${HTTP_SOURCES_DIR}/http.c
	${HTTP_SOURCES_DIR}/http_transaction.c
)
