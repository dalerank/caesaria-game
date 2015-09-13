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

#include "debug_allocators.hpp"

#include <map>
#include <iostream>
#include "utils.hpp"

struct ALLOC_INFO
{
  unsigned int	address;
  unsigned int	size;
  char	file[64];
  unsigned int	line;

  inline bool operator <( const ALLOC_INFO& a )
  {
    return address < a.address;
  }
};

std::map<unsigned int, ALLOC_INFO> allocList;

void debug_new::rem(unsigned int addr)
{
  auto i = allocList.find( addr );
  if( i != allocList.end() )
    allocList.erase( i );
}

void debug_new::add(unsigned int addr, unsigned int asize, const char* fname, unsigned int lnum)
{
  ALLOC_INFO info;
  info.address = addr;
  strncpy(info.file, fname, 63);
  info.line = lnum;
  info.size = asize;

  allocList[ addr] = info;
}


void debug_new::print()
{
  FILE* logFile = fopen( "debug_unfreed.log", "w");

  for( auto i : allocList )
  {
    if( logFile )
    {
      fputs( "Caesaria debug_new print: ", logFile);
      fprintf( logFile,  "ADDR:%d SIZE:%d FILE:%s LINE:%d\n",
               i.second.address, i.second.size,
               i.second.file, i.second.line );
    }
  }
  fflush( logFile );
  fclose( logFile );
}
