project(libpng-static)

file(GLOB PNGLIB_HEADERS_LIST "*.h")
file(GLOB PNGLIB_SOURCES_LIST "*.c")

#set(PNGLIB_NAME_STATIC "libpng")

include_directories( ${ZLIB_HOME} )
add_library(${PNGLIB_NAME} STATIC ${PNGLIB_SOURCES_LIST})
target_link_libraries(${PNGLIB_NAME} ${ZLIB_NAME})
