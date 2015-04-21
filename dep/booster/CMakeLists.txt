project(booster)

file(GLOB BOOSTER_HEADERS_LIST "*.h")
file(GLOB BOOSTER_SOURCES_LIST "*.cpp")

include_directories( ${SDL2_INCLUDE_DIR} )

add_library(${PROJECT_NAME} STATIC ${BOOSTER_SOURCES_LIST})

# Required on Unix OS family to be able to be linked into shared libraries.
set_target_properties(${PROJECT_NAME}
                      PROPERTIES POSITION_INDEPENDENT_CODE ON)

# Expose public includes to other through cache variable.
set(BOOSTER_INCLUDE_DIR ${PROJECT_SOURCE_DIR}
    CACHE INTERNAL "${PROJECT_NAME}: Include Directories" FORCE)

set(BOOSTER_LIBRARY ${PROJECT_NAME}
    CACHE INTERNAL "${PROJECT_NAME}: Library Name" FORCE)
