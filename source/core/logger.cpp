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

#include "logger.hpp"
#include "requirements.hpp"
#include "stringhelper.hpp"

#include <cstdarg>
#include <cfloat>
#include <cstdio>
#include <limits>
#include <climits>
#include <cstring>
#include <iostream>
#include <stdint.h>
#include <fstream>

void Logger::warning( const char* fmt, ... )
{
  va_list argument_list;
  va_start(argument_list, fmt);

  std::string ret;
  StringHelper::vformat( ret, 512, fmt, argument_list );

  va_end(argument_list);

  std::cout << ret << std::endl;
}

void Logger::redirect(std::string filename )
{
  std::ofstream* file = new std::ofstream();

  file->open("stdout.txt");
  std::cout.rdbuf( file->rdbuf() ); // перенапраляем в файл
}

