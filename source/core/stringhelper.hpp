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

#ifndef __CAESARIA_STRINGHELPER_H_INCLUDED__
#define __CAESARIA_STRINGHELPER_H_INCLUDED__

#include <string>
#include <cstdarg>
#include "stringarray.hpp"

class Font;

class StringHelper
{
public:
  typedef enum { equaleIgnoreCase=0, equaleCase } equaleMode;
  static std::string format( unsigned int max_size, const char* fmt, ...);

  static unsigned int hash( unsigned int max_size, const char* fmt, ...);
  static unsigned int hash( const std::string& text );
  static bool startsWith( std::string text, std::string start );

  //! Convert a simple string of base 10 digits into an unsigned 32 bit integer.
  /** \param[in] in: The string of digits to convert. No leading chars are
  allowed, only digits 0 to 9. Parsing stops at the first non-digit.
  \param[out] out: (optional) If provided, it will be set to point at the
  first character not used in the calculation.
  \return The unsigned integer value of the digits. If the string specifies
  too many digits to encode in an u32 then INT_MAX will be returned.
  */
  static unsigned int toUint(const char* in, const char** out=0);

  static unsigned int toUint(const std::string& in);

  static int toInt(const char* in, const char** out=0);

  static int toInt(const std::string& number );

  //! Converts a sequence of digits into a whole positive floating point value.
  /** Only digits 0 to 9 are parsed.  Parsing stops at any other character,
  including sign characters or a decimal point.
  \param in: the sequence of digits to convert.
  \param out: (optional) will be set to point at the first non-converted
  character.
  \return The whole positive floating point representation of the digit
  sequence.
  */
  static float toFloat(const char* in, const char** out = 0);
  static float toFloat(std::string in);

  static bool isEquale( const std::string& a, const std::string& b, equaleMode mode=equaleCase );

  static int compare( const std::string& a, const std::string& b, equaleMode mode=equaleCase );

  static std::string replace(std::string text, const std::string& from, const std::string& to);

  static StringArray split( std::string str, std::string spl );

  static bool isEqualen(const std::string& str1, const std::string& str2, unsigned int n);

  static std::string localeLower( const std::string& str );
  static int vformat(std::string& str, int max_size, const char* format, va_list argument_list);
  static void useStackTrace( bool enabled );

  static std::string trim( const std::string& str );

  static std::string i2str( int valie );
};

#endif //__CAESARIA_STRINGHELPER_H_INCLUDED__
