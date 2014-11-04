# - Locate SDL_ttf library
# This module defines:
#  SDL2_TTF_LIBRARIES, the name of the library to link against
#  SDL2_TTF_INCLUDE_DIRS, where to find the headers
#  SDL2_TTF_FOUND, if false, do not try to link against
#  SDL2_TTF_VERSION_STRING - human-readable string containing the version of SDL_ttf
#
# $SDLDIR is an environment variable that would
# correspond to the ./configure --prefix=$SDLDIR
# used in building SDL.
#
# Created by Eric Wing. This was influenced by the FindSDL.cmake
# module, but with modifications to recognize OS X frameworks and
# additional Unix paths (FreeBSD, etc).

#=============================================================================
# Copyright 2005-2009 Kitware, Inc.
# Copyright 2012 Benjamin Eikel
#
# Distributed under the OSI-approved BSD License (the "License");
# see accompanying file Copyright.txt for details.
#
# This software is distributed WITHOUT ANY WARRANTY; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# See the License for more information.
#=============================================================================
# (To distribute this file outside of CMake, substitute the full
#  License text for the above reference.)

if(NOT SDL2_TTF_INCLUDE_DIR AND SDL2TTF_INCLUDE_DIR)
  set(SDL2_TTF_INCLUDE_DIR ${SDL2TTF_INCLUDE_DIR} CACHE PATH "directory cache
entry initialized from old variable name")
endif()
find_path(SDL2_TTF_INCLUDE_DIR SDL_ttf.h
  HINTS
    ${DEP_SOURCE_DIR}/SDL_ttf
    ENV SDL2TTFDIR
    ENV SDL2DIR
  PATH_SUFFIXES SDL SDL2 include
  PATHS
  /boot/system/develop/headers/x86   #haiku
)

if(NOT SDL2_TTF_LIBRARY AND SDL2TTF_LIBRARY)
  set(SDL2_TTF_LIBRARY ${SDL2TTF_LIBRARY} CACHE FILEPATH "file cache entry
initialized from old variable name")
endif()
find_library(SDL2_TTF_LIBRARY
  NAMES SDL2_ttf
  HINTS
  ENV SDL2TTFDIR
  ENV SDL2DIR
  PATH_SUFFIXES lib lib/x86
  PATHS
  /boot/system/develop		#haiku
  ${DEP_SOURCE_DIR}/SDL_ttf #windows
)

if(SDL2_TTF_INCLUDE_DIR AND EXISTS "${SDL2_TTF_INCLUDE_DIR}/SDL_ttf.h")
  file(STRINGS "${SDL2_TTF_INCLUDE_DIR}/SDL_ttf.h" SDL2_TTF_VERSION_MAJOR_LINE REGEX "^#define[ \t]+SDL2_TTF_MAJOR_VERSION[ \t]+[0-9]+$")
  file(STRINGS "${SDL2_TTF_INCLUDE_DIR}/SDL_ttf.h" SDL2_TTF_VERSION_MINOR_LINE REGEX "^#define[ \t]+SDL2_TTF_MINOR_VERSION[ \t]+[0-9]+$")
  file(STRINGS "${SDL2_TTF_INCLUDE_DIR}/SDL_ttf.h" SDL2_TTF_VERSION_PATCH_LINE REGEX "^#define[ \t]+SDL2_TTF_PATCHLEVEL[ \t]+[0-9]+$")
  string(REGEX REPLACE "^#define[ \t]+SDL2_TTF_MAJOR_VERSION[ \t]+([0-9]+)$" "\\1" SDL2_TTF_VERSION_MAJOR "${SDL2_TTF_VERSION_MAJOR_LINE}")
  string(REGEX REPLACE "^#define[ \t]+SDL2_TTF_MINOR_VERSION[ \t]+([0-9]+)$" "\\1" SDL2_TTF_VERSION_MINOR "${SDL2_TTF_VERSION_MINOR_LINE}")
  string(REGEX REPLACE "^#define[ \t]+SDL2_TTF_PATCHLEVEL[ \t]+([0-9]+)$" "\\1" SDL2_TTF_VERSION_PATCH "${SDL2_TTF_VERSION_PATCH_LINE}")
  set(SDL2_TTF_VERSION_STRING ${SDL2_TTF_VERSION_MAJOR}.${SDL2_TTF_VERSION_MINOR}.${SDL2_TTF_VERSION_PATCH})
  unset(SDL2_TTF_VERSION_MAJOR_LINE)
  unset(SDL2_TTF_VERSION_MINOR_LINE)
  unset(SDL2_TTF_VERSION_PATCH_LINE)
  unset(SDL2_TTF_VERSION_MAJOR)
  unset(SDL2_TTF_VERSION_MINOR)
  unset(SDL2_TTF_VERSION_PATCH)
endif()

set(SDL2_TTF_LIBRARIES ${SDL2_TTF_LIBRARY})
set(SDL2_TTF_INCLUDE_DIRS ${SDL2_TTF_INCLUDE_DIR})

set(SDL2_TTF_FOUND "No")
if (SDL2_TTF_LIBRARY)
   set(SDL2_TTF_FOUND "Yes")
endif()

INCLUDE(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(SDL2_ttf
                                  REQUIRED_VARS SDL2_TTF_LIBRARIES SDL2_TTF_INCLUDE_DIRS
                                  VERSION_VAR SDL2_TTF_VERSION_STRING)

# for backward compatiblity
#set(SDL2TTF_LIBRARY ${SDL2_TTF_LIBRARIES})
#set(SDL2TTF_INCLUDE_DIR ${SDL2_TTF_INCLUDE_DIRS})
#set(SDL2TTF_FOUND ${SDL2_TTF_FOUND})

mark_as_advanced(SDL2_TTF_LIBRARY SDL2_TTF_INCLUDE_DIR)
