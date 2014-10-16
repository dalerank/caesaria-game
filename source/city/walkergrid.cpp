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

unsigned int WalkerGrid::_offset( const TilePos& pos )
{
  return ( pos.j() * _size.width() + pos.i() );
}

void WalkerGrid::clear()
{
  foreach(it, _grid)
  {
    (*it).clear();
  }
}

void WalkerGrid::append( WalkerPtr a )
{
  unsigned int offset = _offset( a->pos() );
  if( offset < _gsize )
  {
    _grid[ offset ].push_back( a );
  }
}

void WalkerGrid::resize( Size size )
{
  _size = size;
  _gsize = size.area();
  _grid.resize( _gsize );
}

const Size& WalkerGrid::size() const
{
  return _size;
}

void WalkerGrid::remove( WalkerPtr a)
{
  unsigned int offset = _offset( a->pos() );
  if( offset < _gsize )
  {
    WalkerList& d = _grid[ offset ];
    foreach( it, d )
    {
      if( *it == a )
      {
        d.erase( it );
        return;
      }
    }
  }
}

const WalkerList& WalkerGrid::at( const TilePos& pos)
{
  unsigned int offset = _offset( pos );
  if( offset < _gsize  )
  {
    return _grid[ offset ];
  }

  Logger::warning( "WalkersGrid incorrect at pos [%d,%d]", pos.i(), pos.j() );
  return invalidList;
}

}
