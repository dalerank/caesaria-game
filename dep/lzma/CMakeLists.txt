project(lzma)

file(GLOB LZMA_HEADERS_LIST "*.h")
file(GLOB LZMA_SOURCES_LIST "*.c")

add_library(${PROJECT_NAME} STATIC ${LZMA_SOURCES_LIST})

# Required on Unix OS family to be able to be linked into shared libraries.
set_target_properties(${PROJECT_NAME}
                      PROPERTIES POSITION_INDEPENDENT_CODE ON)

# Expose public includes to other through cache variable.
set(LZMA_INCLUDE_DIR ${PROJECT_SOURCE_DIR}
    CACHE INTERNAL "${PROJECT_NAME}: Include Directories" FORCE)

set(LZMA_LIBRARY ${PROJECT_NAME}
    CACHE INTERNAL "${PROJECT_NAME}: Library Name" FORCE)
