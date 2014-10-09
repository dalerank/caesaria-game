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

void city::WalkerGrid::clear()
{
  foreach( it, _grid )
    {
      it->second.clear();
    }

  _grid.clear();
}

void city::WalkerGrid::append(WalkerPtr &a)
{
  const TilePos& pos = a->pos();
  if( pos.i() >= 0 && pos.j() >= 0 )
    {
      _grid[ gfx::TileHelper::hash( pos ) ].push_back( a );
    }
}

void city::WalkerGrid::remove(WalkerPtr &a)
{
  TilePos pos = a->pos();
  if( pos.i() >= 0 && pos.j() >= 0 )
    {
      WalkerList& d = _grid[ gfx::TileHelper::hash( pos ) ];
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

const WalkerList &city::WalkerGrid::at(TilePos pos)
{
  static WalkerList invalidList;
  if( pos.i() >= 0 && pos.j() >= 0 )
    {
      if (_grid.find( gfx::TileHelper::hash(pos)) != _grid.end())
        return _grid[ gfx::TileHelper::hash( pos ) ];
    }
  else
    {
      Logger::warning( "WalkersGrid incorrect" );
    }
  return invalidList;
}

#include "walkergrid.hpp"
