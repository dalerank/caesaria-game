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

#include "utils.hpp"

#include <cstdarg>
#include <cfloat>
#include <cstdio>
#include <climits>
#include <cstring>
#include <iostream>
#include <stdint.h>
#include <sstream>

#include "requirements.hpp"
#include "logger.hpp"
#include "variant_list.hpp"
#include "stacktrace.hpp"

namespace utils
{

static bool outputStacktraceLog = false;

int vformat(std::string& str, int max_size, const char* format, va_list argument_list)
{
  if (format == NULL || format[0] == '\0') //au: VladRassokhin
  {
    return 0;
  }

  const int INTERNAL_BUFFER_SIZE = 1024;
  static char buffer[INTERNAL_BUFFER_SIZE];
  char* buffer_ptr = buffer;

  if (max_size + 1 > INTERNAL_BUFFER_SIZE)
    buffer_ptr = new char[max_size + 1];

  int length = vsnprintf(buffer_ptr, max_size, format, argument_list);
  buffer_ptr[length >= 0 ? length : max_size] = '\0';

  if( length <= 0 )
  {
    Logger::warning( "String::vformat: String truncated when processing " + str );
    if( outputStacktraceLog )
      crashhandler::printstack(false);
  }
 
  str = buffer_ptr;

  if (buffer_ptr != buffer)
    delete[] buffer_ptr;

  return length;
}

std::wstring utf8toWString(const char* src, size_t size)
{
  std::wstring dest;

  dest.clear();
  wchar_t w = 0;
  int bytes = 0;
  wchar_t err = L'�';

  for(size_t i = 0; i < size; i++)
  {
    unsigned char c = (unsigned char)src[i];
    if (c <= 0x7f)
    {//first byte
      if (bytes)
      {
        dest.push_back(err);
        bytes = 0;
      }
      dest.push_back((wchar_t)c);
    }
    else if (c <= 0xbf)
    {//second/third/etc byte
      if (bytes)
      {
        w = ((w << 6)|(c & 0x3f));
        bytes--;
        if (bytes == 0)
          dest.push_back(w);
      }
      else
        dest.push_back(err);
    }
    else if (c <= 0xdf)
    {//2byte sequence start
      bytes = 1;
      w = c & 0x1f;
    }
    else if (c <= 0xef)
    {//3byte sequence start
      bytes = 2;
      w = c & 0x0f;
    }
    else if (c <= 0xf7)
    {//3byte sequence start
      bytes = 3;
      w = c & 0x07;
    }
    else
    {
      dest.push_back(err);
      bytes = 0;
    }
  }
  if( bytes )
    dest.push_back(err);

  return dest;
}

void useStackTrace(bool enabled) {  outputStacktraceLog = enabled;}

std::string trim(const std::string& str)
{
  std::string ret = str;
  ret.erase(0, ret.find_first_not_of(' '));       //prefixing spaces
  ret.erase(ret.find_last_not_of(' ')+1);         //surfixing spaces
  return ret;
}

std::string i2str(int value)
{
  return format( 0xff, "%d", value );
}

std::string format( unsigned int max_size, const char* fmt, ...)
{
  std::string ret;
  try
  {
    va_list argument_list;
    va_start(argument_list, fmt);

    vformat( ret, max_size, fmt, argument_list);

    va_end(argument_list);
  }
  catch(...)
  {
    ret = "error_on_format_text";
  }

  return ret;
}

float toFloat( const char* in, const char** out /*= 0*/ )
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

  ++in; //this char need colon or point
  float floatValue = (float)intValue;

  // If there are any digits left to parse, then we need to use
  // floating point arithmetic from here.
  float limiter=10;
  while ( ( *in >= '0') && ( *in <= '9' ) )
  {
    floatValue += (float)(*in - '0') / limiter;
    ++in;
    limiter *= 10.f;
    if (floatValue > FLT_MAX) // Just give up.
      break;
  }

  if (out)
    *out = in;

  return floatValue;
}

float toFloat(std::string in)
{
  return toFloat( in.c_str() );
}

int toInt( const char* in, const char** out/*=0*/ )
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

int toInt( const std::string& number )
{
  return toInt( number.c_str() );
}

unsigned int toUint( const char* in, const char** out/*=0*/ )
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

std::string replace( std::string text, const std::string& from, const std::string& to )
{
  for(size_t i = 0; (i = text.find(from, i)) != std::string::npos; i += to.length())
    text.replace(i, from.length(), to);

  return text;
}

bool isEquale( const std::string& a, const std::string& b, equaleMode mode )
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

int compare( const std::string& a, const std::string& b, equaleMode mode )
{
  switch( mode )
  {
#if defined(_MSC_VER)
  case equaleIgnoreCase: return _stricmp( a.c_str(), b.c_str() );
  case equaleCase: return strcmp( a.c_str(), b.c_str() );
#else
  case equaleIgnoreCase: return strcasecmp( a.c_str(), b.c_str() );
  case equaleCase: return strcmp( a.c_str(), b.c_str() );
#endif
  default: return 0;
  }
}

unsigned int hash( unsigned int max_size, const char* fmt, ... )
{
  va_list argument_list;
  va_start(argument_list, fmt);

  std::string fmtStr;
  vformat( fmtStr, max_size, fmt, argument_list);

  va_end(argument_list);

  return Hash( fmtStr );
}

StringArray split( std::string str, std::string spl )
{
  StringArray ret;
  if(spl.empty())
    return ret;

  std::string::size_type start = 0;
  std::string::size_type pos = str.find_first_of(spl, start);
  while(pos != std::string::npos)
  {
    if(pos != start) // ignore empty tokens
       ret.push_back( str.substr( start, pos - start ));
     start = pos + 1;
     pos = str.find_first_of(spl, start);
  }

  if( start < str.length() ) // ignore trailing delimiter
     ret.push_back( str.substr( start, str.length() - start ) ); // add what's left of the string

  return ret;
}

bool isEqualen( const std::string& str1, const std::string& str2, unsigned int n )
{
  unsigned int i;
  unsigned int minStrLenght = math::min<unsigned int>( str1.length(), str2.length() );
  n = math::min<unsigned int>( n, minStrLenght );
  for(i=0; i < n && str1[i] && str2[i]; ++i)
  {
    if (str1[i] != str2[i])
    {
      return false;
    }
  }

  // if one (or both) of the strings was smaller then they
  // are only equal if they have the same length
  return (i == n) || (str1[i] == 0 && str2[i] == 0);
}

static char __localeLower( char x )
{
  return x >= 'A' && x <= 'Z' ? x + 0x20 : x;
}

std::string localeLower( const std::string& str)
{
  std::string ret = str;
  std::transform( ret.begin(), ret.end(), ret.begin(), __localeLower );
  return ret;
}


bool startsWith(std::string text, std::string start)
{
  text.resize( start.length() );
  return text == start;
}

unsigned int toUint(const std::string& in)
{
  return toUint( in.c_str(), 0 );
}

float eventProbability(float probability, int k, int n)
{
  int limit = math::random( n ) * (1 - probability);
  return k > limit ? 1 : 0;

  /*probability = math::clamp<float>( probability, 0, 1);
  k = math::clamp( k, 0, n );
  float q = 1 - probability;

  float npq = n * probability * q;
  float t = (k - n*probability)/sqrt(npq);
  float res = (1 / sqrt( 2* math::PI * npq )) * exp( -pow(t,2)/2 ) ;

  return res;*/
}

std::string trim(const std::string &str, const std::string &tr)
{
  return replace( str, tr, "" );
}

VariantList toVList(const StringArray &items)
{
  VariantList ret;
  foreach( it, items ) ret << *it;
  return ret;
}

std::string toRoman(int value)
{
  struct romandata_t { int value; char const* numeral; };
  static romandata_t const romandata[] =
     { 1000, "M",
        900, "CM",
        500, "D",
        400, "CD",
        100, "C",
         90, "XC",
         50, "L",
         40, "XL",
         10, "X",
          9, "IX",
          5, "V",
          4, "IV",
          1, "I",
          0, NULL }; // end marker

  std::string result;
  for (romandata_t const* current = romandata; current->value > 0; ++current)
  {
    while (value >= current->value)
    {
      result += current->numeral;
      value  -= current->value;
    }
  }
  return result;
}

/*
 *
#include <string>
#include <cstdlib>
#include <iostream>
#include <cassert>
#include <algorithm>
#include <vector>
#include <ctime>

std::string allowed_chars = " ABCDEFGHIJKLMNOPQRSTUVWXYZ";

// class selection contains the fitness function, encapsulates the
// target string and allows access to it's length. The class is only
// there for access control, therefore everything is static. The
// string target isn't defined in the function because that way the
// length couldn't be accessed outside.
class selection
{
public:
  // this function returns 0 for the destination string, and a
  // negative fitness for a non-matching string. The fitness is
  // calculated as the negated sum of the circular distances of the
  // string letters with the destination letters.
  static int fitness(std::string candidate)
  {
    assert(target.length() == candidate.length());

    int fitness_so_far = 0;

    for (int i = 0; i < target.length(); ++i)
    {
      int target_pos = allowed_chars.find(target[i]);
      int candidate_pos = allowed_chars.find(candidate[i]);
      int diff = std::abs(target_pos - candidate_pos);
      fitness_so_far -= std::min(diff, int(allowed_chars.length()) - diff);
    }

    return fitness_so_far;
  }

  // get the target string length
  static int target_length() { return target.length(); }
private:
  static std::string target;
};

std::string selection::target = "METHINKS IT IS LIKE A WEASEL";

// helper function: cyclically move a character through allowed_chars
void move_char(char& c, int distance)
{
  while (distance < 0)
    distance += allowed_chars.length();
  int char_pos = allowed_chars.find(c);
  c = allowed_chars[(char_pos + distance) % allowed_chars.length()];
}

// mutate the string by moving the characters by a small random
// distance with the given probability
std::string mutate(std::string parent, double mutation_rate)
{
  for (int i = 0; i < parent.length(); ++i)
    if (std::rand()/(RAND_MAX + 1.0) < mutation_rate)
    {
      int distance = std::rand() % 3 + 1;
      if(std::rand()%2 == 0)
        move_char(parent[i], distance);
      else
        move_char(parent[i], -distance);
    }
  return parent;
}

// helper function: tell if the first argument is less fit than the
// second
bool less_fit(std::string const& s1, std::string const& s2)
{
  return selection::fitness(s1) < selection::fitness(s2);
}

int main()
{
  int const C = 100;

  std::srand(time(0));

  std::string parent;
  for (int i = 0; i < selection::target_length(); ++i)
  {
    parent += allowed_chars[std::rand() % allowed_chars.length()];
  }

  int const initial_fitness = selection::fitness(parent);

  for(int fitness = initial_fitness;
      fitness < 0;
      fitness = selection::fitness(parent))
  {
    std::cout << parent << ": " << fitness << "\n";
    double const mutation_rate = 0.02 + (0.9*fitness)/initial_fitness;
    typedef std::vector<std::string> childvec;
    childvec childs;
    childs.reserve(C+1);

    childs.push_back(parent);
    for (int i = 0; i < C; ++i)
      childs.push_back(mutate(parent, mutation_rate));

    parent = *std::max_element(childs.begin(), childs.end(), less_fit);
  }
  std::cout << "final string: " << parent << "\n";
}
*/

/*
 * forest fire
 class forest
{
public:
    forest()
    {
  _bmp.create( MAX_SIDE, MAX_SIDE );
  initForest( 0.05f, 0.005f );
    }

    void initForest( float p, float f )
    {
  _p = p; _f = f;
  seedForest();
    }

    void mainLoop()
    {
  display();
  simulate();
    }

    void setHWND( HWND hwnd ) { _hwnd = hwnd; }

private:
    float probRand() { return ( float )rand() / 32768.0f; }

    void display()
    {
  HDC bdc = _bmp.getDC();
  DWORD clr;

  for( int y = 0; y < MAX_SIDE; y++ )
  {
      for( int x = 0; x < MAX_SIDE; x++ )
      {
    switch( _forest[x][y] )
    {
        case FIRE: clr = 255; break;
        case TREE: clr = RGB( 0, 255, 0 ); break;
        default: clr = 0;
    }

    SetPixel( bdc, x, y, clr );
      }
  }

  HDC dc = GetDC( _hwnd );
  BitBlt( dc, 0, 0, MAX_SIDE, MAX_SIDE, _bmp.getDC(), 0, 0, SRCCOPY );
  ReleaseDC( _hwnd, dc );
    }

    void seedForest()
    {
  ZeroMemory( _forestT, sizeof( _forestT ) );
  ZeroMemory( _forest, sizeof( _forest ) );
  for( int y = 0; y < MAX_SIDE; y++ )
      for( int x = 0; x < MAX_SIDE; x++ )
    if( probRand() < _p ) _forest[x][y] = TREE;
    }

    bool getNeighbors( int x, int y )
    {
  int a, b;
  for( int yy = -1; yy < 2; yy++ )
      for( int xx = -1; xx < 2; xx++ )
      {
    if( !xx && !yy ) continue;
    a = x + xx; b = y + yy;
    if( a < MAX_SIDE && b < MAX_SIDE && a > -1 && b > -1 )
    if( _forest[a][b] == FIRE ) return true;
      }

  return false;
    }

    void simulate()
    {
  for( int y = 0; y < MAX_SIDE; y++ )
  {
      for( int x = 0; x < MAX_SIDE; x++ )
      {
    switch( _forest[x][y] )
    {
        case FIRE: _forestT[x][y] = NONE; break;
        case NONE: if( probRand() < _p ) _forestT[x][y] = TREE; break;
        case TREE: if( getNeighbors( x, y ) || probRand() < _f ) _forestT[x][y] = FIRE;
    }
      }
  }

  for( int y = 0; y < MAX_SIDE; y++ )
      for( int x = 0; x < MAX_SIDE; x++ )
    _forest[x][y] = _forestT[x][y];
    }

    myBitmap _bmp;
    HWND     _hwnd;
    BYTE     _forest[MAX_SIDE][MAX_SIDE], _forestT[MAX_SIDE][MAX_SIDE];
    float    _p, _f;
};
//--------------------------------------------------------------------------------------------------
*/


}//end namespace utils
