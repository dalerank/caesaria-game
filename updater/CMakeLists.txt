project(CaesarIA-updater)

# Find revision ID and hash of the sourcetree
set(BASE_SOURCE_DIR "${CMAKE_SOURCE_DIR}/source")
set(DEP_DIR "${CMAKE_SOURCE_DIR}/dep")

aux_source_directory(. UPD_SRC_LIST)
file(GLOB UPD_INC_LIST "*.hpp")

add_definitions(-DGAME_DISABLED_TILEMAPCONFIG)

set( UPD_DEP_SOURCES_LIST
      "${BASE_SOURCE_DIR}/core/hash.cpp"
      "${BASE_SOURCE_DIR}/core/utils.cpp"
      "${BASE_SOURCE_DIR}/core/bytearray.cpp"
      "${BASE_SOURCE_DIR}/core/logger.cpp"
      "${BASE_SOURCE_DIR}/core/time.cpp"
      "${BASE_SOURCE_DIR}/core/format.cpp"
      "${BASE_SOURCE_DIR}/gfx/tilepos.cpp"
      "${BASE_SOURCE_DIR}/thread/safethread.cpp"
      "${BASE_SOURCE_DIR}/core/variant.cpp"
      "${BASE_SOURCE_DIR}/core/variant_map.cpp"
      "${BASE_SOURCE_DIR}/core/variant_list.cpp"
      "${BASE_SOURCE_DIR}/vfs/directory.cpp"
      "${BASE_SOURCE_DIR}/vfs/file.cpp"
      "${BASE_SOURCE_DIR}/vfs/fileinfo.cpp"
      "${BASE_SOURCE_DIR}/vfs/path.cpp"
      "${BASE_SOURCE_DIR}/vfs/entries.cpp"
      "${BASE_SOURCE_DIR}/vfs/filesystem.cpp"
      "${BASE_SOURCE_DIR}/vfs/memfile.cpp"
      "${BASE_SOURCE_DIR}/vfs/filenative_impl.cpp"
      "${BASE_SOURCE_DIR}/core/stacktrace.cpp"
      "${BASE_SOURCE_DIR}/core/osystem.cpp"
)

file(GLOB HTTP_SOURCES_LIST "${CMAKE_CURRENT_SOURCE_DIR}/http/*.*")

include_directories( ${BASE_SOURCE_DIR} )
include_directories( ${DEP_DIR} )

add_executable(${PROJECT_NAME}
               ${UPD_SRC_LIST} ${UPD_INC_LIST}
               ${UPD_DEP_SOURCES_LIST}
               ${HTTP_SOURCES_LIST} )

if( BUILD_CURL )
  add_definitions( -DCURL_STATICLIB )
  add_definitions( -DCURL_DISABLE_POP3 )
  add_definitions( -DCURL_DISABLE_SMTP )
  include_directories(${CURL_INCLUDE_DIR})
  target_link_libraries(${PROJECT_NAME} ${CURL_LIBRARY})
endif()

find_package(CURL)
if (CURL_FOUND)
  include_directories(${CURL_INCLUDE_DIR})
  target_link_libraries(${PROJECT_NAME} ${CURL_LIBRARY})
endif()

if(WIN32)
  target_link_libraries( ${PROJECT_NAME} psapi ${CURL_LIBRARY} ws2_32 )
  set(EXECUTABLE_FILENAME "updater")
endif(WIN32)

if(HAIKU)
  target_link_libraries( ${PROJECT_NAME} root network )
  set(EXECUTABLE_FILENAME "updater.haiku")
endif(HAIKU)

if(UNIX)
  target_link_libraries( ${PROJECT_NAME} ${CURL_LIBRARY} )
  target_link_libraries( ${PROJECT_NAME} pthread )
  set(EXECUTABLE_FILENAME "updater.linux")
endif(UNIX)

if(APPLE)
  set(EXECUTABLE_FILENAME "updater.macos")
endif(APPLE)

set_property(TARGET ${PROJECT_NAME} PROPERTY OUTPUT_NAME ${EXECUTABLE_FILENAME})

if(CMAKE_BUILD_TYPE STREQUAL "Release")
  message("CaesarIA-updater: building in release mode")
  if(NOT APPLE)
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -s")
  endif()
else()
  message("CaesarIA-updater: building in debug mode")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS_DEBUG} -g")
endif(CMAKE_BUILD_TYPE STREQUAL "Release")

if(WIN32)
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static -static-libgcc -static-libstdc++")
  target_link_libraries(${PROJECT_NAME} "imagehlp")
endif(WIN32)

# prepare working directory
set(UPDATER_BINARY_FILENAME ${EXECUTABLE_FILENAME})
if(WIN32)
  set(UPDATER_BINARY_FILENAME ${UPDATER_BINARY_FILENAME}.exe)
endif(WIN32)

if(NOT MSVC)
ADD_CUSTOM_COMMAND(
    TARGET ${PROJECT_NAME}
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_BINARY_DIR}/${UPDATER_BINARY_FILENAME}" "${WORK_DIR}/${UPDATER_BINARY_FILENAME}"
)
else()
  set(EXECUTABLE_OUTPUT_PATH ${WORK_DIR})
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
