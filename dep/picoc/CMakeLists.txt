project(picoc)

file(GLOB PICOC_HEADERS_LIST "*.h")
file(GLOB PICOC_SOURCES_LIST "*.c")

add_library(${PROJECT_NAME} STATIC ${PICOC_SOURCES_LIST})

# Required on Unix OS family to be able to be linked into shared libraries.
set_target_properties(${PROJECT_NAME}
                      PROPERTIES POSITION_INDEPENDENT_CODE ON)

if(NOT CMAKE_BUILD_TYPE_RELEASE AND NOT MSVC)
  message ("mujs built in debug mode")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS_DEBUG} -g")
endif()

# Expose public includes to other through cache variable.
set(PICOC_INCLUDE_DIR ${PROJECT_SOURCE_DIR}
    CACHE INTERNAL "${PROJECT_NAME}: Include Directories" FORCE)

set(PICOC_LIBRARY ${PROJECT_NAME}
    CACHE INTERNAL "${PROJECT_NAME}: Library Name" FORCE)
