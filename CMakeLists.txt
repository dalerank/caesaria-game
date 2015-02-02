project(CaesarIA)

cmake_minimum_required(VERSION 2.6)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules" ${CMAKE_MODULE_PATH})
message("CMake module path is ${CMAKE_MODULE_PATH}")

message("System is ${CMAKE_SYSTEM_NAME}")
option( BUILD_ZLIB  "Static zlib"    ON )
option( BUILD_AES   "Static aes"     ON )
option( BUILD_BZIP  "Static bzip"    ON )
option( BUILD_LZMA  "Static lzma"    ON )
option( BUILD_CURL  "Static curl"    ON )
option( BUILD_PNG   "Static png"     ON )
option( BUILD_SMK   "Static smk"     ON )
option( BUILD_FREETYPE "Static freetype2" ON )
option( BUILD_TTF   "Static ttf"     ON )
option( BUILD_GL_FRAMEBUFFER "Opengl framebuffer"     ON )
option( BUILD_AUDIO "Use sdl_mixer"  ON )
option( DEBUG_TIMERS "Show debug timers" ON)
option( USE_CPP11 "Build with c++11 standard" OFF)
option( USE_STEAM "Build steam" OFF)
option( SYSTEM_DEPS "Use system-installed dependencies (if found)" OFF)

set(DEP_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/dep" )
set(GAME_CONFIG_DIR "${CMAKE_CURRENT_SOURCE_DIR}/models" )
set(LOCALE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/locale" )
set(STEAM_API_DIR "${CMAKE_CURRENT_SOURCE_DIR}/steam" )
get_filename_component(WORK_DIR ${CMAKE_CURRENT_SOURCE_DIR} PATH)
set(WORK_DIR ${WORK_DIR}/caesaria-test)

if(USE_STEAM)
  add_definitions(-DCAESARIA_USE_STEAM)
endif(USE_STEAM)

if(BUILD_AUDIO)
  set( CAESARIA_USE_SDL_MIXER ON)
  message("Note: SDL audio is enabled (default)")
  message(" ")
else()
  set( CAESARIA_USE_SDL_MIXER OFF)
  message("Note: SDL audio is disabled")
  message(" ")
endif(BUILD_AUDIO)

# Search for appropriate packages on linux, osx and not compile them
if(UNIX AND NOT CYGWIN AND SYSTEM_DEPS)

  find_package( ZLIB )
  if ( ZLIB_FOUND )
    set(BUILD_ZLIB FALSE)
  endif()

  find_package( CURL )
  if (CURL_FOUND)
    set(BUILD_CURL FALSE)
  endif()

  find_package( PNG )
  if (PNG_FOUND)
    set(BUILD_PNG FALSE)
  endif()

  find_package( Freetype )
  if (FREETYPE_FOUND)
    set(BUILD_FREETYPE FALSE)
  endif()

  find_package( SDL2_ttf )
  if (SDL2_TTF_FOUND)
    set(BUILD_TTF FALSE)
  endif()

endif()

if( BUILD_ZLIB )
  set(ZLIB_NAME "zlib")
  set(ZLIB_HOME "${DEP_SOURCE_DIR}/${ZLIB_NAME}" )
  add_subdirectory(${ZLIB_HOME})
endif()

if( BUILD_SMK )
  set(SMKLIB_NAME "smk")
  set(SMKLIB_HOME "${DEP_SOURCE_DIR}/${SMKLIB_NAME}" )
  add_subdirectory(${SMKLIB_HOME})
endif()

if( BUILD_AES )
  set(AESLIB_NAME "aes")
  set(AESLIB_HOME "${DEP_SOURCE_DIR}/${AESLIB_NAME}" )
  add_subdirectory(${AESLIB_HOME})
endif()

if( BUILD_BZIP )
  set(BZIPLIB_NAME "bzip2")
  set(BZIPLIB_HOME "${DEP_SOURCE_DIR}/${BZIPLIB_NAME}" )
  add_subdirectory(${BZIPLIB_HOME})
endif()

if( BUILD_FREETYPE )
  set(FREETYPELIB_NAME "freetype")
  set(FREETYPELIB_HOME "${DEP_SOURCE_DIR}/${FREETYPELIB_NAME}" )
  add_subdirectory(${FREETYPELIB_HOME})
endif()

if( BUILD_TTF )
  set(TTFLIB_NAME "ttf")
  set(TTFLIB_HOME "${DEP_SOURCE_DIR}/${TTFLIB_NAME}" )
  add_subdirectory(${TTFLIB_HOME})
endif()

if( BUILD_LZMA )
  set(LZMALIB_NAME "lzma")
  set(LZMALIB_HOME "${DEP_SOURCE_DIR}/${LZMALIB_NAME}" )
  add_subdirectory(${LZMALIB_HOME})
endif()

if( BUILD_CURL )
  set(CURLLIB_NAME "curl")
  set(CURLLIB_HOME "${DEP_SOURCE_DIR}/${CURLLIB_NAME}" )
  add_subdirectory(${CURLLIB_HOME})
endif()

if( BUILD_PNG )
  set(PNGLIB_NAME "libpng")
  set(PNGLIB_HOME "${DEP_SOURCE_DIR}/${PNGLIB_NAME}" )
  add_subdirectory(${PNGLIB_HOME})
endif()

# Include individual projects
message("")
# We always build game otherwise we would miss the generated header
message("\nBuilding CaesarIA-game")
add_subdirectory(source source)

message("\nBuilding CaesarIA-updater")
add_subdirectory(updater updater)

message("\nBuilding AveC3-addon")
add_subdirectory(addon/ave_c3 addon/ave_c3)

#set(NO_USE_SYSTEM_ZLIB ON)
