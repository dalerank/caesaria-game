// This file is part of CaesarIA.
//
// CaesarIA is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// CaesarIA is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with CaesarIA.  If not, see <http://www.gnu.org/licenses/>.
//
// Copyright 2012-2013 Dalerank, dalerankn8@gmail.com

#ifndef __GAME_PLATFORM_H_INCLUDED__
#define __GAME_PLATFORM_H_INCLUDED__

#if defined(__WIN32__) || defined(_WIN32)
  #define GAME_PLATFORM_WIN
  #define GAME_PLATFORM_WIN32
  #define GAME_PLATFORM_NAME "win32"
#elif defined(WIN64) || defined(_WIN64)
  #define GAME_PLATFORM_WIN
  #define GAME_PLATFORM_WIN64
  #define GAME_PLATFORM_NAME "win64"
#elif defined(__APPLE_CC__) || defined(__APPLE__) || defined(__APPLE_CPP__) || defined(__MACOS_CLASSIC__)
  #define GAME_PLATFORM_UNIX
  #define GAME_PLATFORM_MACOSX
  #define GAME_PLATFORM_NAME "macosx"
#elif defined(__FreeBSD__) || defined(__OpenBSD__)
  #define GAME_PLATFORM_UNIX
  #define GAME_PLATFORM_XBSD
  #define GAME_PLATFORM_NAME "freebsd"
#elif defined(__HAIKU__) || defined(HAIKU)
  #define GAME_PLATFORM_BEOS
  #define GAME_PLATFORM_HAIKU
  #define GAME_PLATFORM_NAME "haiku"
#elif defined(ANDROID)
  #define GAME_PLATFORM_UNIX
  #define GAME_PLATFORM_ANDROID
  #define GAME_PLATFORM_NAME "android"
#else
  #define GAME_PLATFORM_UNIX
  #define GAME_PLATFORM_LINUX
  #define GAME_PLATFORM_NAME "linux"
#endif

#endif //__GAME_PLATFORM_H_INCLUDED__
