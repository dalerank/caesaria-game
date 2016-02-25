project(CaesarIA-tileset)

# Find revision ID and hash of the sourcetree
set(BASE_SOURCE_DIR "${CMAKE_SOURCE_DIR}/source")
set(DEP_DIR "${CMAKE_SOURCE_DIR}/dep")

aux_source_directory(. TILESET_SRC_LIST)
add_definitions(-DGAME_DISABLE_PICTUREBANK)
add_definitions(-DGAME_DISABLED_TILEMAPCONFIG)
file(GLOB TILESET_INC_LIST "*.hpp")

set( TILESET_DEP_SOURCES_LIST
      "${BASE_SOURCE_DIR}/core/hash.cpp"
      "${BASE_SOURCE_DIR}/core/utils.cpp"
      "${BASE_SOURCE_DIR}/core/bytearray.cpp"
      "${BASE_SOURCE_DIR}/core/logger.cpp"
      "${BASE_SOURCE_DIR}/core/time.cpp"
      "${BASE_SOURCE_DIR}/core/json.cpp"
      "${BASE_SOURCE_DIR}/core/format.cpp"
      "${BASE_SOURCE_DIR}/gfx/tilepos.cpp"
      "${BASE_SOURCE_DIR}/core/saveadapter.cpp"
      "${BASE_SOURCE_DIR}/core/variant.cpp"
      "${BASE_SOURCE_DIR}/core/variant_map.cpp"
      "${BASE_SOURCE_DIR}/core/variant_list.cpp"
      "${BASE_SOURCE_DIR}/core/position_array.cpp"
      "${BASE_SOURCE_DIR}/core/eventconverter.cpp"
      "${BASE_SOURCE_DIR}/core/font.cpp"
      "${BASE_SOURCE_DIR}/core/color_list.cpp"
      "${BASE_SOURCE_DIR}/core/debug_timer.cpp"
      "${BASE_SOURCE_DIR}/vfs/directory.cpp"
      "${BASE_SOURCE_DIR}/vfs/file.cpp"
      "${BASE_SOURCE_DIR}/vfs/fileinfo.cpp"
      "${BASE_SOURCE_DIR}/gfx/batch.cpp"
      "${BASE_SOURCE_DIR}/gfx/picture.cpp"
      "${BASE_SOURCE_DIR}/gfx/engine.cpp"
      "${BASE_SOURCE_DIR}/gfx/sdl_batcher.cpp"
      "${BASE_SOURCE_DIR}/gfx/sdl_engine.cpp"
      "${BASE_SOURCE_DIR}/gfx/IMG_savepng.cpp"
      "${BASE_SOURCE_DIR}/gfx/loader_png.cpp"
      "${BASE_SOURCE_DIR}/gfx/loader_bmp.cpp"
      "${BASE_SOURCE_DIR}/gfx/loader.cpp"
      "${BASE_SOURCE_DIR}/vfs/path.cpp"
      "${BASE_SOURCE_DIR}/vfs/entries.cpp"
      "${BASE_SOURCE_DIR}/vfs/filesystem.cpp"
      "${BASE_SOURCE_DIR}/vfs/memfile.cpp"
      "${BASE_SOURCE_DIR}/vfs/filenative_impl.cpp"
      "${BASE_SOURCE_DIR}/core/stacktrace.cpp"
      "${BASE_SOURCE_DIR}/core/osystem.cpp"
      "${BASE_SOURCE_DIR}/core/line.cpp"
      "tileset.model"
)

include_directories( ${BASE_SOURCE_DIR} )
include_directories( ${DEP_DIR} )
include_directories( ${SDL2MINI_INCLUDE_DIR})
include_directories( ${PNG_INCLUDE_DIR})
include_directories( ${SDL2_TTF_INCLUDE_DIR})
include_directories( ${ZLIB_INCLUDE_DIR})
include_directories( ${MINIZIP_INCLUDE_DIR})
add_executable(${PROJECT_NAME}
               ${TILESET_SRC_LIST} ${TILESET_INC_LIST}
               ${TILESET_DEP_SOURCES_LIST} )

target_link_libraries(${PROJECT_NAME} ${PNG_LIBRARY})
target_link_libraries(${PROJECT_NAME} ${ZLIB_LIBRARY})
target_link_libraries(${PROJECT_NAME} ${MINIZIP_LIBRARY})
target_link_libraries(${PROJECT_NAME} ${SDL2MINI_LIBRARY})
target_link_libraries(${PROJECT_NAME} ${SDL2_TTF_LIBRARY})

if(WIN32)
  target_link_libraries(${PROJECT_NAME} "winmm")
  target_link_libraries( ${PROJECT_NAME} psapi )
  set(EXECUTABLE_FILENAME "tileset")
endif(WIN32)

if(HAIKU)
  target_link_libraries( ${PROJECT_NAME} root )
  set(EXECUTABLE_FILENAME "tileset.haiku")
endif(HAIKU)

if(APPLE)
  set(EXECUTABLE_FILENAME "tileset.macos")
endif(APPLE)

if(UNIX AND NOT APPLE)
  target_link_libraries(${PROJECT_NAME} "dl")
  set(EXECUTABLE_FILENAME "tileset.linux")
endif()

set_property(TARGET ${PROJECT_NAME} PROPERTY OUTPUT_NAME ${EXECUTABLE_FILENAME})

if(CMAKE_BUILD_TYPE STREQUAL "Release")
  message("CaesarIA-tileset: building in release mode")
  if(NOT APPLE)
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -s")
  endif()
else()
  message("CaesarIA-tileset: building in debug mode")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS_DEBUG} -g")
endif(CMAKE_BUILD_TYPE STREQUAL "Release")

if(WIN32)
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static -static-libgcc -static-libstdc++")
  target_link_libraries(${PROJECT_NAME} "imagehlp")
endif(WIN32)

# prepare working directory
set(TILESET_BINARY_FILENAME ${EXECUTABLE_FILENAME})
if(WIN32)
  set(TILESET_BINARY_FILENAME ${TILESET_BINARY_FILENAME}.exe)
endif(WIN32)

if(NOT MSVC)
ADD_CUSTOM_COMMAND(
    TARGET ${PROJECT_NAME}
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_BINARY_DIR}/${TILESET_BINARY_FILENAME}" "${WORK_DIR}/tools/tileset/${TILESET_BINARY_FILENAME}"
)

ADD_CUSTOM_COMMAND(
    TARGET ${PROJECT_NAME}
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_SOURCE_DIR}/tileset/tileset.model" "${WORK_DIR}/tools/tileset"
)
else()
  set(EXECUTABLE_OUTPUT_PATH ${WORK_DIR}/tools/tileset)
endif()

# set compiler options
if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_COMPILER_IS_CLANG)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS_DEBUG} -Wall")
  if(NOT HAIKU)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS_DEBUG} -Wno-unused-value")
  endif()
endif()

# Enforce C++11
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
