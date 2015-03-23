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
#include "gfx/helper.hpp"

using namespace gfx;

namespace city
{

ActivePoints::ActivePoints()
{
  resize( maxPoins );
}

TilePos ActivePoints::get( unsigned int index) const
{
  return index < maxPoins ? (*this)[index] : tilemap::invalidLocation();
}

void ActivePoints::set( unsigned int index, const TilePos& pos)
{
  if( index < maxPoins )
    (*this)[index] = pos;
}

VariantList ActivePoints::save() const {  return toVList(); }

void ActivePoints::load(VariantList &stream)
{
  fromVList( stream );
  resize( maxPoins );
}

}//end namespace city
