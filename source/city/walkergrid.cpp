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

#include "walkergrid.hpp"
#include "walker/walker.hpp"
#include "core/logger.hpp"

namespace city
{

static const WalkerList invalidList = WalkerList();

unsigned int WalkersGrid::_offset( const TilePos& pos )
{
  return ( pos.j() * _size.width() + pos.i() );
}

void WalkersGrid::clear()
{
  for( auto& cell : _grid)
    cell.clear();
}

void WalkersGrid::append( WalkerPtr a )
{
  unsigned int offset = _offset( a->pos() );
  if( offset < _gsize )
  {
    _grid[ offset ].push_back( a );
  }
}

void WalkersGrid::resize( Size size )
{
  _size = size;
  _gsize = size.area();
  _grid.resize( _gsize );
}

const Size& WalkersGrid::size() const
{
  return _size;
}

void WalkersGrid::remove( WalkerPtr a)
{
  unsigned int offset = _offset( a->pos() );
  if( offset < _gsize )
  {
    WalkerList& walkers = _grid[ offset ];
    walkers.remove( a );
  }
}

void WalkersGrid::update(const WalkerList& walkers)
{
  clear();
  for( auto wlk : walkers )
    append( wlk );
}

bool compare_zvalue(const WalkerPtr& one, const WalkerPtr& two)
{
  const int a = /*one->mappos().x() -*/ one->mappos().y();
  const int b = /*two->mappos().x() -*/ two->mappos().y();
  return a < b;
}

void WalkersGrid::sort()
{
  for( auto& cell : _grid )
  {
    if( cell.size() > 1 )
    {
      std::sort( cell.begin(), cell.end(), compare_zvalue );
    }
  }
}

const WalkerList& WalkersGrid::at( const TilePos& pos)
{
  unsigned int offset = _offset( pos );
  if( offset < _gsize  )
  {
    return _grid[ offset ];
  }

  Logger::warning( "WalkersGrid incorrect at pos [{0},{1}]", pos.i(), pos.j() );
  return invalidList;
}

}
