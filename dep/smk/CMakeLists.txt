project(smk-static)

file(GLOB SMK_HEADERS_LIST "*.h")
file(GLOB SMK_SOURCES_LIST "*.c")

set(SMKLIB_NAME_STATIC "smk")

add_library(${SMKLIB_NAME_STATIC} STATIC ${SMK_SOURCES_LIST})
