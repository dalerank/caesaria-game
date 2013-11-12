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

#ifndef __OPENCAESAR3_REQUIREMENTS_INCLUDE_
#define __OPENCAESAR3_REQUIREMENTS_INCLUDE_

#include "version.hpp"
#include "platform.hpp"

//see CMakeLists.txt for this define
//#define NO_USE_SYSTEM_ZLIB

#if defined OC3_PLATFORM_WIN
  // alignment of a member was sensitive to packing
  //#pragma warning(disable : 4121)

  // assignment operator could not be generated
  //#pragma warning(disable : 4512)

  #if !defined _CRT_SECURE_NO_DEPRECATE
    #define _CRT_SECURE_NO_DEPRECATE
  #endif

  #if defined(_MSC_VER)
    #define OC3_COMPILER_NAME "msvc"
  #elif defined(__GNUC__)
    #define OC3_COMPILER_NAME "mingw"
  #endif
#endif

#ifndef OC3_COMPILER_NAME
  #define OC3_COMPILER_NAME "unknown"
#endif

#define _USE_ASSERT_4_DEBUG

#if defined(_USE_ASSERT_4_DEBUG)
  #if defined(OC3_PLATFORM_WIN)
    #if defined(_MSC_VER)
        #undef OC3_USE_MINGW_COMPILER
        #if defined(OC3_PLATFORM_WIN64) // using portable common solution for x64 configuration
          #include <crtdbg.h>
          #define _OC3_DEBUG_BREAK_IF( _CONDITION_ ) if (_CONDITION_) {_CrtDbgBreak();}
        #else
          #define _OC3_DEBUG_BREAK_IF( _CONDITION_ ) if (_CONDITION_) {_asm int 3}
        #endif
      #else
        #if defined (__GNUC__)
          #define OC3_USE_MINGW_COMPILER
          #include <cassert>
        #endif
        #define _OC3_DEBUG_BREAK_IF( _CONDITION_ ) assert( !(_CONDITION_) );
    #endif // _MSC_VER
  #else //OC3_PLATFORM_WIN
    #if defined (__GNUC__)
      #include <cassert>
    #endif
    #define _OC3_DEBUG_BREAK_IF( _CONDITION_ ) assert( !(_CONDITION_) );
  #endif
#else
  #define _OC3_DEBUG_BREAK_IF( _CONDITION_ )
#endif

#define PREFEDINE_CLASS_SMARTPOINTER(a) class a; typedef SmartPtr<a> a##Ptr;
#define PREFEDINE_CLASS_SMARTPOINTER_LIST(a,b) PREFEDINE_CLASS_SMARTPOINTER(a); typedef std::list< a##Ptr > a##b;

#define _OC3_DISABLE_COPY(Class) \
    Class( const Class& ); \
    Class& operator=( const Class& );

#endif
