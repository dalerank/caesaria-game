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
// Copyright 2012-2015 Dalerank, dalerankn8@gmail.com

#ifndef CAESARIA_TILEPOS_ARRAY_HPP
#define CAESARIA_TILEPOS_ARRAY_HPP

#include <vector>
#include "position.hpp"
#include "variant.hpp"

class TilePosArray : public std::vector< TilePos >
{
public:
  TilePosArray& operator<<(const TilePos& pos )
  {
    push_back( pos );
    return *this;
  }

  VariantList toVList() const
  {
    VariantList ret;
    foreach( it, *this ) { ret << *it; }
    return ret;
  }

  void fromVList( const VariantList& vlist )
  {
    clear();
    foreach( it, vlist )
      push_back( it->toTilePos() );
  }
};

#endif//CAESARIA_TILEPOS_ARRAY_HPP
