project(bzip-static)

file(GLOB BZIP_HEADERS_LIST "*.h")
file(GLOB BZIP_SOURCES_LIST "*.c")

set(BZIPLIB_NAME_STATIC "bzip2")

add_library(${BZIPLIB_NAME_STATIC} STATIC ${BZIP_SOURCES_LIST})
