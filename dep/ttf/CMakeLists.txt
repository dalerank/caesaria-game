project(ttf)

#find_package(Freetype REQUIRED)
message("FREETYPE_DIR is" ${FREETYPELIB_HOME})
find_package(SDL2 REQUIRED)

file(GLOB TTF_HEADERS_LIST "*.h")
file(GLOB TTF_SOURCES_LIST "*.c")

set(TTFLIB_NAME_STATIC "ttf")

link_directories(${PNGLIB_HOME})

include_directories(
  ${SDL2_INCLUDE_DIR}
  ${FREETYPELIB_HOME}/include
)

add_library(${TTFLIB_NAME_STATIC} STATIC ${TTF_SOURCES_LIST})
target_link_libraries(${TTFLIB_NAME_STATIC} ${FREETYPELIB_NAME})

