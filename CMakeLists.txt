project(CaesarIA)

cmake_minimum_required(VERSION 2.6)

message("System is ${CMAKE_SYSTEM_NAME}")
option( BUILD_ZLIB  "Static zlib"    ON )
option( BUILD_AES   "Static aes"     ON )
option( BUILD_BZIP  "Static bzip"    ON )
option( BUILD_LZMA  "Static lzma"    ON )
option( BUILD_CURL  "Static curl"    ON )
option( BUILD_PNG   "Static png"     ON )
option( BUILD_SMK   "Static smk"     ON )
option( BUILD_GL_FRAMEBUFFER "Opengl framebuffer"     ON )
option( BUILD_GL_SHADERS "OpenGL shaders"     ON )
option( BUILD_AUDIO "Use sdl_mixer"  ON )

set(DEP_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/dep" )
set(GAME_CONFIG_DIR "${CMAKE_CURRENT_SOURCE_DIR}/models" )
set(LOCALE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/locale" )
get_filename_component(WORK_DIR ${CMAKE_CURRENT_SOURCE_DIR} PATH)
set(WORK_DIR ${WORK_DIR}/caesaria-test)

if(BUILD_AUDIO)
  set( CAESARIA_USE_SDL_MIXER ON)
  message("Note: SDL audio is enabled (default)")
  message(" ")
else()
  set( CAESARIA_USE_SDL_MIXER OFF)
  message("Note: SDL audio is disabled")
  message(" ")
endif(BUILD_AUDIO)

if( BUILD_ZLIB )
  set(ZLIB_NAME "zlib")
  set(ZLIB_HOME "${DEP_SOURCE_DIR}/${ZLIB_NAME}" )
  add_subdirectory(${ZLIB_HOME} ${ZLIB_HOME} EXCLUDE_FROM_ALL)
endif()

if( BUILD_SMK )
  set(SMKLIB_NAME "smk")
  set(SMKLIB_HOME "${DEP_SOURCE_DIR}/${SMKLIB_NAME}" )
  add_subdirectory(${SMKLIB_HOME} ${SMKLIB_HOME} EXCLUDE_FROM_ALL)
endif()

if( BUILD_AES )
  set(AESLIB_NAME "aes")
  set(AESLIB_HOME "${DEP_SOURCE_DIR}/${AESLIB_NAME}" )
  add_subdirectory( ${AESLIB_HOME} ${AESLIB_HOME} EXCLUDE_FROM_ALL )
endif()

if( BUILD_BZIP )
  set(BZIPLIB_NAME "bzip2")
  set(BZIPLIB_HOME "${DEP_SOURCE_DIR}/${BZIPLIB_NAME}" )
  add_subdirectory( ${BZIPLIB_HOME} ${BZIPLIB_HOME} EXCLUDE_FROM_ALL )
endif()

if( BUILD_LZMA )
  set(LZMALIB_NAME "lzma")
  set(LZMALIB_HOME "${DEP_SOURCE_DIR}/${LZMALIB_NAME}" )
  add_subdirectory( ${LZMALIB_HOME} ${LZMALIB_HOME} EXCLUDE_FROM_ALL )
endif()

if( BUILD_CURL )
  set(CURLLIB_NAME "curl")
  set(CURLLIB_HOME "${DEP_SOURCE_DIR}/${CURLLIB_NAME}" )
  add_subdirectory( ${CURLLIB_HOME} ${CURLLIB_HOME} EXCLUDE_FROM_ALL )
endif()

if( BUILD_PNG )
  set(PNGLIB_NAME "libpng")
  set(PNGLIB_HOME "${DEP_SOURCE_DIR}/${PNGLIB_NAME}" )
  add_subdirectory( ${PNGLIB_HOME} ${PNGLIB_HOME} EXCLUDE_FROM_ALL )
endif()

# Include individual projects
message("")
# We always build game otherwise we would miss the generated header
message("Building CaesarIA-game")
add_subdirectory(source source)

message("Building CaesarIA-updater")
add_subdirectory(updater updater)

#set(NO_USE_SYSTEM_ZLIB ON)
