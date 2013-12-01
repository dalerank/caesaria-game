project(lzma-static)

file(GLOB LZMA_HEADERS_LIST "*.h")
file(GLOB LZMA_SOURCES_LIST "*.c")

set(LZMALIB_NAME_STATIC "lzma")

add_library(${LZMALIB_NAME_STATIC} STATIC ${LZMA_SOURCES_LIST})
