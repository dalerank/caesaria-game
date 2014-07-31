project(ttf)

find_package(Freetype REQUIRED)
message("FREETYPE_INCLUDE_DIR " ${FREETYPE_INCLUDE_DIR_freetype2})

file(GLOB TTF_HEADERS_LIST "*.h")
file(GLOB TTF_SOURCES_LIST "*.c")

set(TTFLIB_NAME_STATIC "ttf")

include_directories(
  ${SDL2_INCLUDE_DIR}
  ${FREETYPE_INCLUDE_DIR_ft2build}
  ${FREETYPE_INCLUDE_DIR_freetype2}
)

add_library(${TTFLIB_NAME_STATIC} STATIC ${TTF_SOURCES_LIST})
target_link_libraries(${TTFLIB_NAME_STATIC} ${FREETYPE_LIBRARY})

