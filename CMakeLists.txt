project(CaesarIA)

cmake_minimum_required(VERSION 2.6)

option( BUILD_LIBINTL "Static libintl" OFF )

set(UTILS_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/utils" )
set(GAME_CONFIG_DIR "${CMAKE_CURRENT_SOURCE_DIR}/models" )

if( BUILD_LIBINTL )
  set(LIBINTL_VERSION "0.14.4")
  set(LIBINTL_DIR "libintl-${LIBINTL_VERSION}" )
  set(LIBINTL_HOME "${CMAKE_CURRENT_SOURCE_DIR}/utils/${LIBINTL_DIR}" )
  set(LIBINTL_A "${LIBINTL_HOME}/lib/libintl.a" )
  add_subdirectory( ${LIBINTL_HOME} EXCLUDE_FROM_ALL )
endif( BUILD_LIBINTL )

add_subdirectory(source)

#set(NO_USE_SYSTEM_ZLIB ON)
