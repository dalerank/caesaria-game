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

#include "active_points.hpp"
#include "gfx/tilemap_config.hpp"
#include "core/variant_list.hpp"

using namespace gfx;

namespace city
{

ActivePoints::ActivePoints()
{
  resize( maxPoints );
}

TilePos ActivePoints::get( unsigned int index) const
{
  return index < maxPoints ? (*this)[index] : TilePos::invalid();
}

void ActivePoints::set( unsigned int index, const TilePos& pos)
{
  if( index < maxPoints )
    (*this)[index] = pos;
}

VariantList ActivePoints::save() const
{
  return TilePosArray::save();
}

void ActivePoints::load( const VariantList &stream)
{
  TilePosArray::load( stream );
  resize( maxPoints );
}

}//end namespace city
