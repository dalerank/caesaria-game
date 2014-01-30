project(CaesarIA-updater)

# Find revision ID and hash of the sourcetree
set(BASE_SOURCE_DIR "${CMAKE_SOURCE_DIR}/source")
set(DEP_DIR "${CMAKE_SOURCE_DIR}/dep")

aux_source_directory(. UPD_SRC_LIST)
file(GLOB UPD_INC_LIST "*.hpp")

set(UPD_CORE_SOURCES_LIST
      "${BASE_SOURCE_DIR}/core/stringhelper.cpp"
      "${BASE_SOURCE_DIR}/core/bytearray.cpp"
      "${BASE_SOURCE_DIR}/core/logger.cpp"
      "${BASE_SOURCE_DIR}/core/mutex.cpp"
      "${BASE_SOURCE_DIR}/core/time.cpp"
      "${BASE_SOURCE_DIR}/core/thread.cpp"
      "${BASE_SOURCE_DIR}/core/threadevent.cpp"
      "${BASE_SOURCE_DIR}/core/threadtask.cpp"
      "${BASE_SOURCE_DIR}/core/variant.cpp" )

file(GLOB UPD_VFS_SOURCES_LIST "${BASE_SOURCE_DIR}/vfs/*.*")
file(GLOB HTTP_SOURCES_LIST "${CMAKE_CURRENT_SOURCE_DIR}/http/*.*")

include_directories( ${BASE_SOURCE_DIR} )
include_directories( ${DEP_DIR} )

add_executable(${PROJECT_NAME}
               ${UPD_SRC_LIST} ${UPD_INC_LIST}
               ${UPD_CORE_SOURCES_LIST} ${UPD_VFS_SOURCES_LIST}
               ${HTTP_SOURCES_LIST} )

if( BUILD_ZLIB )
  link_directories(${ZLIB_HOME})
  target_link_libraries(${PROJECT_NAME} ${ZLIB_NAME})
endif()

if( BUILD_AES )
  link_directories(${AESLIB_HOME})
  target_link_libraries(${PROJECT_NAME} ${AESLIB_NAME})
endif()

if( BUILD_BZIP )
  link_directories(${BZIPLIB_HOME})
  target_link_libraries(${PROJECT_NAME} ${BZIPLIB_NAME})
endif()

if( BUILD_LZMA )
  link_directories(${LZMALIB_HOME})
  target_link_libraries(${PROJECT_NAME} ${LZMALIB_NAME})
endif()

if( BUILD_CURL )
  add_definitions( -DCURL_STATICLIB )
  add_definitions( -DCURL_DISABLE_POP3 )
  add_definitions( -DCURL_DISABLE_SMTP )
  link_directories(${CURLLIB_HOME})

  target_link_libraries(${PROJECT_NAME} ${CURLLIB_NAME})
endif()

if(WIN32)
  target_link_libraries( ${PROJECT_NAME} psapi ws2_32 ${CURLLIB_NAME} )
  set(EXECUTABLE_FILENAME "updater")
endif(WIN32)

if(LINUX)
  target_link_libraries( ${PROJECT_NAME} ${CURLLIB_NAME} )
  target_link_libraries( ${PROJECT_NAME} pthread )

  set(EXECUTABLE_FILENAME "updater.linux")
endif(LINUX)

if(HAIKU)
  target_link_libraries( ${PROJECT_NAME} ${CURLLIB_NAME} )
  target_link_libraries( ${PROJECT_NAME} root )

  set(EXECUTABLE_FILENAME "updater.haiku")
endif(HAIKU)

if(APPLE)
  set(EXECUTABLE_FILENAME "updater.macos")
endif(APPLE)

set_property(TARGET ${PROJECT_NAME} PROPERTY OUTPUT_NAME ${EXECUTABLE_FILENAME})

if(CMAKE_BUILD_TYPE STREQUAL "Release")
  message("CaesarIA-updater: building in release mode")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -s")
else()
  message("CaesarIA-updater: building in debug mode")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS_DEBUG} -g")
endif(CMAKE_BUILD_TYPE STREQUAL "Release")

if(WIN32)
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libgcc -static-libstdc++ -static")
endif(WIN32)

# prepare working directory
set(UPDATER_BINARY_FILENAME ${EXECUTABLE_FILENAME})
if(WIN32)
  set(UPDATER_BINARY_FILENAME ${UPDATER_BINARY_FILENAME}.exe)
endif(WIN32)

ADD_CUSTOM_COMMAND(
    TARGET ${PROJECT_NAME}
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_BINARY_DIR}/${UPDATER_BINARY_FILENAME}" "${WORK_DIR}/${UPDATER_BINARY_FILENAME}"
)

# set compiler options
if(CMAKE_COMPILER_IS_GNUCXX OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS_DEBUG} -Wall -Wno-unused-value")
endif(CMAKE_COMPILER_IS_GNUCXX OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
