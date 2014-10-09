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
// Copyright 2012-2014 Dalerank, dalerankn8@gmail.com

#ifndef __CAESARIA_REQUIREMENTS_INCLUDE_
#define __CAESARIA_REQUIREMENTS_INCLUDE_

#include "version.hpp"
#include "platform.hpp"

#define CAESARIA_STR_EXT(__A) #__A
#define CAESARIA_STR_A(__A) CAESARIA_STR_EXT(__A)

#if defined(CAESARIA_PLATFORM_WIN)
  // alignment of a member was sensitive to packing
  //#pragma warning(disable : 4121)

  // assignment operator could not be generated
  //#pragma warning(disable : 4512)

  #if !defined _CRT_SECURE_NO_DEPRECATE
    #define _CRT_SECURE_NO_DEPRECATE
  #endif
#endif

#ifndef CAESARIA_COMPILER_NAME
  #define CAESARIA_COMPILER_NAME "unknown"
#endif

#define CAESARIA_LITERALCONST(name) const char* lc_##name = CAESARIA_STR_EXT(name);

#define _USE_ASSERT_4_DEBUG

#if defined(_USE_ASSERT_4_DEBUG)
  #if defined(CAESARIA_PLATFORM_WIN)
    #if defined(_MSC_VER)
        #undef CAESARIA_USE_MINGW_COMPILER
        #if defined(CAESARIA_PLATFORM_WIN64) // using portable common solution for x64 configuration
          #include <crtdbg.h>
          #include <cassert>
          #define _CAESARIA_DEBUG_BREAK_IF( _CONDITION_ ) if (_CONDITION_) {_CrtDbgBreak();}
        #else
          #include <cassert>
          #define _CAESARIA_DEBUG_BREAK_IF( _CONDITION_ ) if (_CONDITION_) {_asm int 3}
        #endif
      #else
        #if defined (__GNUC__)
          #define CAESARIA_USE_MINGW_COMPILER
          #include <cassert>
        #endif
        #define _CAESARIA_DEBUG_BREAK_IF( _CONDITION_ ) assert( !(_CONDITION_) );
    #endif // _MSC_VER
  #else //CAESARIA_PLATFORM_WIN
    #if defined (__GNUC__)
      #include <cassert>
    #endif
    #define _CAESARIA_DEBUG_BREAK_IF( _CONDITION_ ) assert( !(_CONDITION_) );
  #endif
#else
  #define _CAESARIA_DEBUG_BREAK_IF( _CONDITION_ )
#endif

#define __DISABLE_COPY(Class) \
  Class( const Class& ); \
  Class& operator=( const Class& );

#define __DECLARE_IMPL(Class) \
  class Impl; \
  ScopedPtr<Impl> _d##Class; \
  const ScopedPtr<Impl>& _dfunc() const { return _d##Class; } \
  ScopedPtr<Impl>& _dfunc() { return _d##Class; } \
  friend class Impl;

#define __INIT_IMPL(Class) _d##Class( new Impl )
#define __D_IMPL(a,Class) ScopedPtr<Impl>& a = _d##Class;
#define __D_IMPL_CONST(a,Class) const ScopedPtr<Impl>& a = _d##Class;

#endif //__CAESARIA_REQUIREMENTS_INCLUDE_
