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

#define OC3_VERSION_MAJOR 0
#define OC3_VERSION_MINOR 0
#define OC3_VERSION_REVSN 289

#define OC3_STR_EXT(__A) #__A
#define OC3_STR_A(__A) OC3_STR_EXT(__A)
#define OC3_VERSION OC3_STR_A(OC3_VERSION_MAJOR)"."OC3_STR_A(OC3_VERSION_MINOR)"."OC3_STR_A(OC3_VERSION_REVSN)

#define _USE_ASSERT_4_DEBUG

#if defined(_USE_ASSERT_4_DEBUG)
  #if defined(_MSC_VER) && !defined (_WIN32_WCE)
    #if defined(WIN64) || defined(_WIN64) // using portable common solution for x64 configuration
      #include <crtdbg.h>
      #define _OC3_DEBUG_BREAK_IF( _CONDITION_ ) if (_CONDITION_) {_CrtDbgBreak();}
    #else
      #define _OC3_DEBUG_BREAK_IF( _CONDITION_ ) if (_CONDITION_) {_asm int 3}
    #endif
  #else
    #if defined (__GNUC__)
      #include <cassert>
    #endif
    #define _OC3_DEBUG_BREAK_IF( _CONDITION_ ) assert( !(_CONDITION_) );
   #endif
#else
  #define _OC3_DEBUG_BREAK_IF( _CONDITION_ )
#endif

#define _OC3_DISABLE_COPY(Class) \
    Class( const Class& ); \
    Class& operator=( const Class& );\

#ifdef _MSC_VER
  #define snprintf _snprintf
#endif

#endif
