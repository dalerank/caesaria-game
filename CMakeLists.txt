project(CaesarIA)

cmake_minimum_required(VERSION 2.6)

option( BUILD_LIBINTL "Static libintl" OFF )
option( BUILD_ZLIB "Static zlib" ON )
option( BUILD_AES "Static aes" ON )
option( BUILD_BZIP "Static bzip" ON )
option( BUILD_LZMA "Static lzma" ON )
option( BUILD_CURL "Static curl" ON )

set(DEP_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/dep" )
set(GAME_CONFIG_DIR "${CMAKE_CURRENT_SOURCE_DIR}/models" )

if( BUILD_LIBINTL )
  set(LIBINTL_VERSION "0.14.4")
  set(LIBINTL_DIR "libintl-${LIBINTL_VERSION}" )
  set(LIBINTL_HOME "${DEP_SOURCE_DIR}/${LIBINTL_DIR}" )
  set(LIBINTL_A "${LIBINTL_HOME}/lib/libintl.a" )
  add_subdirectory( ${LIBINTL_HOME} EXCLUDE_FROM_ALL )
endif( BUILD_LIBINTL )

if( BUILD_ZLIB )
  set(ZLIB_NAME "zlib")
  set(ZLIB_HOME "${DEP_SOURCE_DIR}/${ZLIB_NAME}" )
  add_subdirectory( ${ZLIB_HOME} EXCLUDE_FROM_ALL )
endif()

if( BUILD_AES )
  set(AESLIB_NAME "aes")
  set(AESLIB_HOME "${DEP_SOURCE_DIR}/${AESLIB_NAME}" )
  add_subdirectory( ${AESLIB_HOME} EXCLUDE_FROM_ALL )
endif()

if( BUILD_BZIP )
  set(BZIPLIB_NAME "bzip2")
  set(BZIPLIB_HOME "${DEP_SOURCE_DIR}/${BZIPLIB_NAME}" )
  add_subdirectory( ${BZIPLIB_HOME} EXCLUDE_FROM_ALL )
endif()

if( BUILD_LZMA )
  set(LZMALIB_NAME "lzma")
  set(LZMALIB_HOME "${DEP_SOURCE_DIR}/${LZMALIB_NAME}" )
  add_subdirectory( ${LZMALIB_HOME} EXCLUDE_FROM_ALL )
endif()

if( BUILD_CURL )
  set(CURLLIB_NAME "curl")
  set(CURLLIB_HOME "${DEP_SOURCE_DIR}/${CURLLIB_NAME}" )
  add_subdirectory( ${CURLLIB_HOME} EXCLUDE_FROM_ALL )
endif()

add_subdirectory(source)
add_subdirectory(updater)

#set(NO_USE_SYSTEM_ZLIB ON)
