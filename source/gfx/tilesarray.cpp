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

#include "tilesarray.hpp"

namespace gfx
{

bool TilesArray::contain(TilePos tilePos) const
{
  foreach( it, *this )
  {
    if( (*it)->epos() == tilePos )
      return true;
  }

  return false;
}

TilesArray::TilesArray(const TilesArray& a)
{
  *this = a;
}

TilePos TilesArray::leftUpCorner() const
{
  if( empty() )
    return TilePos( -1, -1 );

  TilePos ret( 9999, 0 );
  foreach( it, *this )
  {
    const TilePos& cpos = (*it)->epos();
    if( cpos.i() < ret.i() ) { ret.setI( cpos.i() ); }
    if( cpos.j() > ret.j() ) { ret.setJ( cpos.j() ); }
  }

  return ret;
}

TilePos TilesArray::rightDownCorner() const
{
  if( empty() )
    return TilePos( -1, -1 );

  TilePos ret( 0, 9999 );
  foreach( it, *this )
  {
    const TilePos& cpos = (*it)->epos();
    if( cpos.j() < ret.j() ) { ret.setJ( cpos.j() ); }
    if( cpos.i() > ret.i() ) { ret.setI( cpos.i() ); }
  }

  return ret;
}

TilesArray&TilesArray::operator=(const TilesArray& a)
{
  clear();
  if( a.size() == 1 )
    push_back( a.front() );
  else
    insert(begin(), a.begin(), a.end());
  return *this;
}

TilesArray&TilesArray::append(const TilesArray& a)
{
  insert( end(), a.begin(), a.end() );

  return *this;
}

TilesArray TilesArray::walkableTiles(bool alllands) const
{
  TilesArray ret;
  foreach( i, *this)
  {
    if( (*i)->isWalkable( alllands ) )
      ret.push_back( *i );
  }

  return ret;
}

TilesArray& TilesArray::remove( const TilePos& pos)
{
  foreach( it, *this )
  {
    if( (*it)->pos() == pos )
    {
      erase( it );
      break;
    }
  }

  return *this;
}

TileOverlayList TilesArray::overlays() const
{
  TileOverlayList ret;
  foreach( i, *this)
  {
    if( (*i)->overlay().isValid() )
      ret << (*i)->overlay();
  }

  return ret;
}

Tile*TilesArray::random() const
{
  return size() > 0 ? (*this)[ math::random( size() ) ] : 0;
}



}//end namespace
