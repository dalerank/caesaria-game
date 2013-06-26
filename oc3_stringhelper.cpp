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

#include "oc3_stringhelper.hpp"
#include "oc3_requirements.hpp"

#include <cstdarg>
#include <cfloat>
#include <cstdio>
#include <limits>
#include <climits>
#include <cstring>
#include <iostream>
#include <stdint.h>

static int formatString(std::string& str, int max_size, const char* format, va_list argument_list)
{
  const int INTERNAL_BUFFER_SIZE = 1024;
  static char buffer[INTERNAL_BUFFER_SIZE];
  char* buffer_ptr = buffer;

  if (max_size + 1 > INTERNAL_BUFFER_SIZE)
    buffer_ptr = new char[max_size + 1];

  int length = vsnprintf(buffer_ptr, max_size, format, argument_list);
  buffer_ptr[length >= 0 ? length : max_size] = '\0';

  _OC3_DEBUG_BREAK_IF( length == -1 && "String::sprintf: String truncated when processing " );
 
  str = buffer_ptr;

  if (buffer_ptr != buffer)
    delete[] buffer_ptr;

  return length;
}

std::string StringHelper::format( unsigned int max_size, const char* fmt, ...)
{
  va_list argument_list;
  va_start(argument_list, fmt);

  std::string ret;
  int length = formatString( ret, max_size, fmt, argument_list);

  va_end(argument_list);

  return ret;
}

float StringHelper::toFloat( const char* in, const char** out /*= 0*/ )
{
  if (!in)
  {
    if (out)
      *out = in;
    return 0.f;
  }

  const unsigned int MAX_SAFE_U32_VALUE = UINT_MAX / 10 - 10;
  unsigned int intValue = 0;

  // Use integer arithmetic for as long as possible, for speed
  // and precision.
  while ( ( *in >= '0') && ( *in <= '9' ) )
  {
    // If it looks like we're going to overflow, bail out
    // now and start using floating point.
    if (intValue >= MAX_SAFE_U32_VALUE)
      break;

    intValue = (intValue * 10) + (*in - '0');
    ++in;
  }

  float floatValue = (float)intValue;

  // If there are any digits left to parse, then we need to use
  // floating point arithmetic from here.
  while ( ( *in >= '0') && ( *in <= '9' ) )
  {
    floatValue = (floatValue * 10.f) + (float)(*in - '0');
    ++in;
    if (floatValue > FLT_MAX) // Just give up.
      break;
  }

  if (out)
    *out = in;

  return floatValue;
}

int StringHelper::toInt( const char* in, const char** out/*=0*/ )
{
  if (!in)
  {
    if (out)
      *out = in;
    return 0;
  }

  const bool negative = ('-' == *in);
  if (negative || ('+' == *in))
    ++in;

  const unsigned int unsignedValue = toUint( in, out );
  if (unsignedValue > (unsigned int)INT_MAX)
  {
    if (negative)
      return INT_MIN;
    else
      return INT_MAX;
  }
  else
  {
    if (negative)
      return -((int)unsignedValue);
    else
      return (int)unsignedValue;
  }
}

unsigned int StringHelper::toUint( const char* in, const char** out/*=0*/ )
{
  if (!in)
  {
    if (out)
      *out = in;
    return 0;
  }

  bool overflow=false;
  unsigned int  unsignedValue = 0;
  while ( ( *in >= '0') && ( *in <= '9' ))
  {
    const unsigned tmp = ( unsignedValue * 10 ) + ( *in - '0' );
    if (tmp<unsignedValue)
    {
      unsignedValue=(unsigned int)0xffffffff;
      overflow=true;
    }
    if (!overflow)
      unsignedValue = tmp;
    ++in;
  }

  if (out)
    *out = in;

  return unsignedValue;
}

std::string StringHelper::replace( std::string text, const std::string& from, const std::string& to )
{
  for (size_t i = 0; (i = text.find(from, i)) != std::string::npos; i += to.length())
    text.replace(i, from.length(), to);

  //std::cout << text << std::endl;

  return text;
}

bool StringHelper::isEquale( const std::string& a, const std::string& b, equaleMode mode )
{
  switch( mode )
  {
#if defined(_MSC_VER)
  case equaleIgnoreCase: return !_stricmp( a.c_str(), b.c_str() );
  case equaleCase: return !strcmp( a.c_str(), b.c_str() );
#else
  case equaleIgnoreCase: return !strcasecmp( a.c_str(), b.c_str() );
  case equaleCase: return !strcmp( a.c_str(), b.c_str() );
#endif
  default: return false;
  }
}

void StringHelper::debug( unsigned int max_size, const char* fmt, ... )
{
  va_list argument_list;
  va_start(argument_list, fmt);

  std::string ret;
  int length = formatString( ret, max_size, fmt, argument_list);

  va_end(argument_list);

  std::cout << ret << std::endl;
}

unsigned int StringHelper::hash( const std::string& text )
{
  unsigned int nHash = 0;
  const char* key = text.c_str();
  if( key )
  {
    while(*key)
      nHash = (nHash<<5) + nHash + *key++;
  }

  return nHash;
}

unsigned int StringHelper::hash( unsigned int max_size, const char* fmt, ... )
{
  va_list argument_list;
  va_start(argument_list, fmt);

  std::string fmtStr;
  int length = formatString( fmtStr, max_size, fmt, argument_list);

  va_end(argument_list);

  return hash( fmtStr );
}