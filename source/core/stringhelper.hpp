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

#ifndef __OPENCAESAR3_STRINGHELPER_H_INCLUDED__
#define __OPENCAESAR3_STRINGHELPER_H_INCLUDED__

#include <string>
#include <vadefs.h>
#include "core/stringarray.hpp"

class StringHelper
{
public:
  typedef enum { equaleIgnoreCase=0, equaleCase } equaleMode;
  static std::string format( unsigned int max_size, const char* fmt, ...);

  static unsigned int hash( unsigned int max_size, const char* fmt, ...);
  static unsigned int hash( const std::string& text );

  //! Convert a simple string of base 10 digits into an unsigned 32 bit integer.
  /** \param[in] in: The string of digits to convert. No leading chars are
  allowed, only digits 0 to 9. Parsing stops at the first non-digit.
  \param[out] out: (optional) If provided, it will be set to point at the
  first character not used in the calculation.
  \return The unsigned integer value of the digits. If the string specifies
  too many digits to encode in an u32 then INT_MAX will be returned.
  */
  static unsigned int toUint(const char* in, const char** out=0);

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

  static bool isEquale( const std::string& a, const std::string& b, equaleMode mode=equaleCase );

  static std::string replace(std::string text, const std::string& from, const std::string& to);

  static StringArray split( const std::string& str, const std::string& spl, unsigned int count=1, bool ignoreEmptyTokens=true, bool keepSeparators=false);

  static bool isEqualen(const std::string& str1, const std::string& str2, unsigned int n);

  static std::string localeLower( const std::string& str );
  static char localeLower( char x );

  static int vformat(std::string& str, int max_size, const char* format, va_list argument_list);
};

#endif
