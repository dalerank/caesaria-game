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

#ifndef __CAESARIA_PLATFORMTYPES_H_INCLUDED__
#define __CAESARIA_PLATFORMTYPES_H_INCLUDED__

#include "platform.hpp"

#if defined(__UINT32_MAX__) || defined(UINT32_MAX)
  #include <inttypes.h>
#else
  // Unsigned base types.
  typedef unsigned char       uint8;		// 8-bit  unsigned.
  typedef unsigned short int	uint16;		// 16-bit unsigned.
  typedef unsigned long       uint32;		// 32-bit unsigned.
  typedef unsigned long long	uint64;		// 64-bit unsigned.

  // Signed base types.
  typedef	signed char         int8;		  // 8-bit  signed.
  typedef signed short int    int16;		// 16-bit signed.
  typedef signed long         int32;		// 32-bit signed.
  typedef signed long long    int64;		// 64-bit signed.
#endif

#endif //__CAESARIA_PLATFORMTYPES_H_INCLUDED__
