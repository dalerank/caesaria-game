project(zlib-static)

file(GLOB Z_HEADERS_LIST "*.h")
file(GLOB Z_SOURCES_LIST "*.c")

#set(ZLIB_NAME_STATIC "zlib")

add_library(${ZLIB_NAME} STATIC ${Z_SOURCES_LIST})
