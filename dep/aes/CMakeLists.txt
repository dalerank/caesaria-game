project(aes-static)

file(GLOB AES_HEADERS_LIST "*.h")
file(GLOB AES_SOURCES_LIST "*.cpp")

set(AESLIB_NAME_STATIC "aes")

add_library(${AESLIB_NAME_STATIC} STATIC ${AES_SOURCES_LIST})
