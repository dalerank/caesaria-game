project(AveC3-addon)

# Find revision ID and hash of the sourcetree
SET (LIB_TYPE SHARED)

set(BASE_SOURCE_DIR "${CMAKE_SOURCE_DIR}/source")
set(DEP_DIR "${CMAKE_SOURCE_DIR}/dep")

add_definitions(-DCAESARIA_ADDON_DEFINED)

aux_source_directory(. SOURCES_LIST)
file(GLOB HEADERS_LIST "*.hpp")

set( DEP_SOURCES_LIST

)

set(ADDON_FILENAME "avec3.addon")
include_directories( ${BASE_SOURCE_DIR} )

# Create a target for the library
ADD_LIBRARY(AveC3 ${LIB_TYPE} ${SOURCES_LIST} ${HEADERS_LIST} ${DEP_SOURCES_LIST} )

if(CMAKE_BUILD_TYPE STREQUAL "Release")
  message("AveC3-addon: building in release mode")
  if(NOT APPLE)
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -s")
  endif()
else()
  message("AveC3-addon: building in debug mode")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS_DEBUG} -g")
endif(CMAKE_BUILD_TYPE STREQUAL "Release")

if(WIN32)
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static -static-libgcc -static-libstdc++")
endif(WIN32)

# prepare working directory
ADD_CUSTOM_COMMAND(
    TARGET ${PROJECT_NAME}
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_BINARY_DIR}/${ADDON_FILENAME}" "${WORK_DIR}/addon/${ADDON_FILENAME}"
)

if(EXT_BINDIR)
  message( "Addon AveC3: ext bindir=${CMAKE_SOURCE_DIR}/${EXT_BINDIR}")
  ADD_CUSTOM_COMMAND(
      TARGET ${PROJECT_NAME}
      POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_BINARY_DIR}/${ADDON_FILENAME}" "${CMAKE_SOURCE_DIR}/${EXT_BINDIR}/addons/${ADDON_FILENAME}"
  )
endif(EXT_BINDIR)

# set compiler options
if(CMAKE_COMPILER_IS_GNUCXX OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS_DEBUG} -Wall")
  if(NOT HAIKU)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS_DEBUG} -Wno-unused-value")
  endif(NOT HAIKU)
endif(CMAKE_COMPILER_IS_GNUCXX OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
