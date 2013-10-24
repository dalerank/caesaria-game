// This file is part of openCaesar3.
//
// openCaesar3 is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// openCaesar3 is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with openCaesar3.  If not, see <http://www.gnu.org/licenses/>.

#ifndef __OPENCAESAR3_PLATFORM_H_INCLUDED__
#define __OPENCAESAR3_PLATFORM_H_INCLUDED__

#if defined(__WIN32__) || defined(_WIN32)
  #define OC3_PLATFORM_WIN
  #define OC3_PLATFORM_WIN32
  #define OC3_PLATFORM_NAME "win32"
#elif defined(WIN64) || defined(_WIN64)
  #define OC3_PLATFORM_WIN
  #define OC3_PLATFORM_WIN64
  #define OC3_PLATFORM_NAME "win64"
#elif defined(__APPLE_CC__)
  #define OC3_PLATFORM_UNIX
  #define OC3_PLATFORM_MACOSX
  #define OC3_PLATFORM_NAME "macosx"
#elif defined(__FreeBSD__) || defined(__OpenBSD__)
  #define OC3_PLATFORM_UNIX
  #define OC3_PLATFORM_XBSD
  #define OC3_PLATFORM_NAME "freebsd"
#else
  #define OC3_PLATFORM_UNIX
  #define OC3_PLATFORM_LINUX
  #define OC3_PLATFORM_NAME "linux"
#endif

#endif //__OPENCAESAR3_PLATFORM_H_INCLUDED__
