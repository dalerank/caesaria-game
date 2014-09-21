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
// Timo Bingmann from http://idlebox.net/
// Copyright 2012-2014 Dalerank, dalerankn8@gmail.com

#include "stacktrace.hpp"

#include <stdlib.h>
#if defined(CAESARIA_PLATFORM_LINUX) && !defined(CAESARIA_PLATFORM_ANDROID)
  #include <execinfo.h>
#endif

#if !defined(CAESARIA_PLATFORM_HAIKU) && !defined(CAESARIA_PLATFORM_ANDROID) && !defined(_MSC_VER)
  #include <cxxabi.h>
#endif

#include "scopedptr.hpp"
#include "bytearray.hpp"
#include "logger.hpp"

/** Print a demangled stack backtrace of the caller function to FILE* out. */
void Stacktrace::print(unsigned int starting_frame, unsigned int max_frames)
{
#if defined(CAESARIA_PLATFORM_LINUX) && !defined(CAESARIA_PLATFORM_ANDROID)
  Logger::warning("Stacktrace::begin :");

  // storage array for stack trace address data
  void* addrlist[max_frames+1];

  // retrieve current stack addresses
  int addrlen = backtrace(addrlist, sizeof(addrlist) / sizeof(void*));

  if (addrlen == 0)
  {
    Logger::warning(" <empty, possibly corrupt>\n");
    return;
  }

  // resolve addresses into strings containing "filename(function+address)",
  // this array must be free()-ed
  char** symbollist = backtrace_symbols(addrlist, addrlen);

  // allocate string which will be filled with the demangled function name
  size_t funcnamesize = 256;
  ByteArray funcname;
  funcname.resize( funcnamesize );

  // iterate over the returned symbol lines. skip the first, it is the
  // address of this function.
  for (int i = starting_frame + 1; i < addrlen; i++)
  {
    char *begin_name = 0, *begin_offset = 0, *end_offset = 0;

    // find parentheses and +address offset surrounding the mangled name:
    // ./module(function+0x15c) [0x8048a6d]
    for (char *p = symbollist[i]; *p; ++p)
    {
      if (*p == '(') begin_name = p;
      else if (*p == '+') begin_offset = p;
      else if (*p == ')' && begin_offset)
      {
        end_offset = p;
        break;
      }
    }

    if (begin_name && begin_offset && end_offset
    && begin_name < begin_offset)
    {
      *begin_name++ = '\0';
      *begin_offset++ = '\0';
      *end_offset = '\0';

      // mangled name is now in [begin_name, begin_offset) and caller
      // offset in [begin_offset, end_offset). now apply
      // __cxa_demangle():

      int status;
      char* ret = abi::__cxa_demangle(begin_name, funcname.data(), &funcnamesize, &status);
      if (status == 0)
      {
        funcname = ret; // use possibly realloc()-ed string
        Logger::warning(" %s : %s+%s", symbollist[i], funcname.data(), begin_offset);
      }
      else
      {
        // demangling failed. Output function name as a C function with
        // no arguments.
        Logger::warning(" %s : %s()+%s", symbollist[i], begin_name, begin_offset);
      }
    }
    else
    {
      // couldn't parse the line? print the whole line.
      Logger::warning(" %s", symbollist[i]);
    }
  }

  //free(funcname);
  free(symbollist);
#else
  Logger::warning("Stack trace not available");
#endif // CAESARIA_PLATFORM_LINUX
}



