#ifndef __OPENCAESAR3_STRINGHELPER_H_INCLUDED__
#define __OPENCAESAR3_STRINGHELPER_H_INCLUDED__

#include <string>

class StringHelper
{
public:
  static std::string format( unsigned int max_size, const char* fmt, ...);

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

  static std::string replace(std::string text, const std::string& from, const std::string& to);
};

#endif