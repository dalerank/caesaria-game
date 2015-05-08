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

#include "hash.hpp"
static const unsigned int hash_seed = 1315423911;

Hash::Hash( const std::string& text )
{
  unsigned int nHash = 0;
  const char* key = text.c_str();
  if( key )
  {
    nHash = hash_seed;
    while(*key)
    {
      nHash ^= ((nHash << 5) + *key + (nHash >> 2));
      key++;
    }

    nHash &= 0x7FFFFFFF;
  }

  _value = nHash;
}
