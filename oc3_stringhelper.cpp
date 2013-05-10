#include "oc3_stringhelper.hpp"
#include "oc3_requirements.hpp"

#include <stdarg.h>
#include <float.h>

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