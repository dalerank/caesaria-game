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
#include "gfx/tilemap_config.hpp"
#include "objects/overlay.hpp"

namespace gfx
{

bool TilesArray::contain(const TilePos &tilePos) const
{
  for( auto tile : *this )
  {
    if( tile->epos() == tilePos )
      return true;
  }

  return false;
}

bool TilesArray::contain(Tile* a) const
{
  for( auto tile : *this )
  {
    if( tile == a )
      return true;
  }

  return false;
}

Tile* TilesArray::find(const TilePos& tilePos) const
{
  for( auto tile : *this )
  {
    if( tile->epos() == tilePos )
      return tile;
  }

  return 0;
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

  for( auto tile : *this )
  {
    const TilePos& cpos = tile->epos();

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
    return TilePos::invalid();

  TilePos ret( INT_MAX, 0 );
  for( auto tile : *this )
  {
    const TilePos& cpos = tile->epos();
    if( cpos.i() < ret.i() ) { ret.setI( cpos.i() ); }
    if( cpos.j() > ret.j() ) { ret.setJ( cpos.j() ); }
  }

  return ret;
}

TilePos TilesArray::rightDownCorner() const
{
  if( empty() )
    return TilePos::invalid();

  TilePos ret( 0, INT_MAX );
  for( auto tile : *this )
  {
    const TilePos& cpos = tile->epos();
    if( cpos.j() < ret.j() ) { ret.setJ( cpos.j() ); }
    if( cpos.i() > ret.i() ) { ret.setI( cpos.i() ); }
  }

  return ret;
}

TilesArray& TilesArray::operator=(const TilesArray& a)
{
  clear();
  if( a.size() == 1 )
    push_back( a.front() );
  else
    insert(begin(), a.begin(), a.end());
  return *this;
}

TilesArray& TilesArray::append(const TilesArray& a)
{
  insert( end(), a.begin(), a.end() );

  return *this;
}

TilesArray &TilesArray::append(Tile *a)
{
  push_back( a );
  return *this;
}

bool TilesArray::appendOnce(Tile* a)
{
  if( contain( a ) )
    return false;

  push_back( a );
  return true;
}

TilesArray TilesArray::walkables(bool alllands) const
{
  TilesArray ret;
  for( auto tile : *this)
  {
    if( tile->isWalkable( alllands ) )
      ret.push_back( tile );
  }

  return ret;
}

TilesArray TilesArray::select(Tile::Type flag) const
{
  TilesArray ret;
  for( auto tile : *this )
    if( tile->getFlag( flag ) )
      ret.push_back( tile );

  return ret;
}

TilesArray TilesArray::select(Tile::Param param) const
{
  TilesArray ret;
  for( auto tile : *this )
    if( tile->param( param ) )
      ret.push_back( tile );

  return ret;
}

int TilesArray::count(Tile::Type flag) const
{
  int result=0;
  for( auto tile : *this )
    if( tile->getFlag( flag ) )
      result++;

  return result;
}

TilesArray TilesArray::terrains() const
{
  TilesArray ret;
  for( auto tile : *this)
  {
    if( tile->getFlag( Tile::tlWater ) || tile->getFlag( Tile::tlDeepWater )
        || tile->getFlag( Tile::tlRock ) || tile->getFlag( Tile::tlCoast )
        || tile->getFlag( Tile::tlRift) )
      continue;

    ret.push_back( tile );
  }

  return ret;
}

TilesArray TilesArray::masters() const
{
  TilesArray masterTiles;
  for( auto tile : *this )
    if( tile->master() != 0 )
      masterTiles.appendOnce( tile->master() );

  return masterTiles;
}

TilesArray TilesArray::children(Tile* master) const
{
  TilesArray ret;
  for( auto tile : *this )
    if( tile->master() == master )
      ret.push_back( tile );

  return ret;
}

TilesArray TilesArray::waters() const
{
  TilesArray ret;
  for( auto tile : *this)
  {
    if( tile->getFlag( Tile::tlWater ) || tile->getFlag( Tile::tlDeepWater ) )
      ret.push_back( tile );
  }

  return ret;
}

TilesArray& TilesArray::remove( const TilePos& pos)
{
  for( iterator it=begin(); it!=end(); )
  {
    if( (*it)->pos() == pos ) { it = erase( it ); }
    else { ++it; }
  }

  return *this;
}

PointsArray TilesArray::mappositions() const
{
  PointsArray ret;
  for( auto tile : *this )
    ret.push_back(tile->mappos());

  return ret;
}

Locations TilesArray::locations() const
{
  Locations ret;
  for( auto tile : *this )
    ret << tile->pos();

  return ret;
}

OverlayList TilesArray::overlays() const
{
  OverlayList ret;
  for( auto tile : *this )
    ret.addIfValid( tile->overlay() );

  return ret;
}

void TilesArray::pop_front() { erase( this->begin() ); }

Tile* TilesArray::random() const
{
  return size() > 0 ? (*this)[ math::random( size()-1 ) ] : nullptr;
}

}//end namespace
