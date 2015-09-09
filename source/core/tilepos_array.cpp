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

#include "tilepos_array.hpp"
#include "variant_list.hpp"

TilePosArray& TilePosArray::operator<<(const TilePos& pos)
{
  push_back( pos );
  return *this;
}

TilePosArray& TilePosArray::addIfNot(const TilePos& pos)
{
  for( auto&& tpos : *this )
    if( tpos == pos )
      return *this;

  push_back( pos );
  return *this;
}

TilePosArray& TilePosArray::append(const TilePos &pos)
{
  push_back( pos );
  return *this;
}

TilePos TilePosArray::valueOrEmpty(unsigned int index)
{
  return index < size() ? at( index ) : TilePos();
}

VariantList TilePosArray::save() const
{
  VariantList ret;
  for( auto pos : *this ) { ret << pos; }
  return ret;
}

void TilePosArray::load(const VariantList &vlist)
{
  clear();
  for( auto&& it : vlist )
    push_back( it.toTilePos() );
}

TilePosSet& TilePosSet::operator<<(const TilePos &pos)
{
  insert( pos );
  return *this;
}

TilePos TilePosArray::random()
{
  return valueOrEmpty( math::random( size() - 1 ) );
}
