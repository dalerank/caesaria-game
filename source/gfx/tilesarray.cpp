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
#include "objects/overlay.hpp"
#include "gfx/helper.hpp"

namespace gfx
{

bool TilesArray::contain(const TilePos &tilePos) const
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

TilesArray::Corners TilesArray::corners() const
{
  if( empty() )
    return Corners();

  Corners ret;
  ret.leftup    = TilePos( INT_MAX, 0 );
  ret.leftdown  = TilePos( INT_MAX, INT_MAX );
  ret.rightdown = TilePos( 0, INT_MAX );
  ret.rightup   = TilePos( 0, 0 );
  foreach( it, *this )
  {
    const TilePos& cpos = (*it)->epos();

    if( cpos.i() < ret.leftup.i() ) { ret.leftup.setI( cpos.i() ); }
    if( cpos.j() > ret.leftup.j() ) { ret.leftup.setJ( cpos.j() ); }

    if( cpos.j() < ret.rightdown.j() ) { ret.rightdown.setJ( cpos.j() ); }
    if( cpos.i() > ret.rightdown.i() ) { ret.rightdown.setI( cpos.i() ); }

    if( cpos.j() < ret.leftdown.j() ) { ret.leftdown.setJ( cpos.j() ); }
    if( cpos.i() < ret.leftdown.i() ) { ret.leftdown.setI( cpos.i() ); }

    if( cpos.j() > ret.rightup.j() ) { ret.rightup.setJ( cpos.j() ); }
    if( cpos.i() > ret.rightup.i() ) { ret.rightup.setI( cpos.i() ); }
  }

  return ret;
}

TilePos TilesArray::leftUpCorner() const
{
  if( empty() )
    return gfx::tilemap::invalidLocation();

  TilePos ret( INT_MAX, 0 );
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
    return gfx::tilemap::invalidLocation();

  TilePos ret( 0, INT_MAX );
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

TilesArray &TilesArray::append(Tile *a)
{
  push_back( a );
  return *this;
}

TilesArray TilesArray::walkables(bool alllands) const
{
  TilesArray ret;
  foreach( i, *this)
  {
    if( (*i)->isWalkable( alllands ) )
      ret.push_back( *i );
  }

  return ret;
}

TilesArray TilesArray::terrains() const
{
  TilesArray ret;
  foreach( i, *this)
  {
    if( (*i)->getFlag( Tile::tlWater ) || (*i)->getFlag( Tile::tlDeepWater )
        || (*i)->getFlag( Tile::tlRock ) || (*i)->getFlag( Tile::tlCoast )
        || (*i)->getFlag( Tile::tlRift) )
      continue;

    ret.push_back( *i );
  }

  return ret;
}

TilesArray TilesArray::waters() const
{
  TilesArray ret;
  foreach( i, *this)
  {
    if( (*i)->getFlag( Tile::tlWater ) || (*i)->getFlag( Tile::tlDeepWater ) )
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

TilePosArray TilesArray::locations() const
{
  TilePosArray ret;
  foreach( it, *this )
    ret << (*it)->pos();

  return ret;
}

OverlayList TilesArray::overlays() const
{
  OverlayList ret;
  foreach( i, *this )
    ret.addIfValid( (*i)->overlay() );

  return ret;
}

void TilesArray::pop_front() { erase( this->begin() ); }

Tile*TilesArray::random() const
{
  return size() > 0 ? (*this)[ math::random( size() ) ] : 0;
}



}//end namespace
