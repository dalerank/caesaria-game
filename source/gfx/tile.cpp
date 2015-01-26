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

#include "tile.hpp"
#include "core/exception.hpp"
#include "objects/building.hpp"
#include "tileoverlay.hpp"
#include "game/resourcegroup.hpp"
#include "core/utils.hpp"
#include "core/logger.hpp"
#include "helper.hpp"
#include "game/gamedate.hpp"

using namespace constants;

namespace gfx
{

namespace
{
  static Animation invalidAnimation;
}

void Tile::Terrain::reset()
{
  clearFlags();
  params[ Tile::pDesirability ] = 0;
  params[ Tile::pWellWater ] = 0;
  params[ Tile::pFountainWater ] = 0;
  params[ Tile::pReservoirWater ] = 0;
}

void Tile::Terrain::clearFlags()
{
  water      = false;
  rock       = false;
  tree       = false;
  road       = false;
  coast      = false;
  elevation  = false;
  garden     = false;
  meadow     = false;
  wall       = false;
  rubble     = false;  
  deepWater  = false;
}

Tile::Tile( const TilePos& pos) //: _terrain( 0, 0, 0, 0, 0, 0 )
{
  _pos = pos;
  _master = NULL;
  _wasDrawn = false;
  _overlay = NULL;
  _terrain.reset();
  _terrain.imgid = 0;
  _height = 0;
  setEPos( pos );
}

int Tile::i() const    {   return _pos.i();   }
int Tile::j() const    {   return _pos.j();   }
void Tile::setPicture(const Picture& picture) {  _picture = picture; }
void Tile::setPicture(const char* rc, const int index){ setPicture( Picture::load( rc, index ) );}
void Tile::setPicture(const std::string& name){ setPicture( Picture::load( name ) );}

const Picture& Tile::picture() const {  return _picture; }
Tile* Tile::masterTile() const{  return _master;}
void Tile::setMasterTile(Tile* master){  _master = master; }

bool Tile::isFlat() const
{
  if( _master && _master != this )
  {
    return _master->isFlat();
  }

  return (_overlay.isValid()
           ? _overlay->isFlat()
           : !(_terrain.rock || _terrain.elevation || _terrain.tree) );
}


Point Tile::center() const {  return Point( _epos.i(), _epos.j() ) * tilemap::cellSize().height() + tilemap::cellCenter(); }
bool Tile::isMasterTile() const{  return (_master == this);}
void Tile::setEPos(const TilePos& epos)
{
  _epos = epos;
  _mappos = Point( tilemap::cellSize().width() * ( _epos.i() + _epos.j() ),
                   tilemap::cellSize().height() * ( _epos.i() - _epos.j() ) - _height * tilemap::cellSize().height() );
}

void Tile::changeDirection(Tile *masterTile, constants::Direction newDirection)
{
  if( masterTile && _overlay.isValid() )
  {
    _overlay->changeDirection( masterTile, newDirection);
  }

  if( _terrain.coast )
  {
    int iid = tile::turnCoastTile( _terrain.imgid, newDirection );

    _picture = iid == -1
                ? Picture::getInvalid()
                : imgid::toPicture( iid );
  }
}


void Tile::animate(unsigned int time)
{
  if( _overlay.isNull() && _animation.isValid() )
  {
    _animation.update( time );
  }
}

const Animation& Tile::animation() const{  return _overlay.isValid() ? invalidAnimation : _animation; }
void Tile::setAnimation(const Animation& animation){ _animation = animation;}

bool Tile::isWalkable( bool alllands ) const
{
  bool walkable = (_terrain.road || (alllands && !_terrain.deepWater && !_terrain.water && !_terrain.tree && !_terrain.rock));
  if( _overlay.isValid() )
  {
    walkable &= _overlay->isWalkable();
  }

  return walkable;
}

bool Tile::getFlag(Tile::Type type) const
{
  switch( type )
  {
  case tlRoad: return _terrain.road;
  case tlWater: return _terrain.water;
  case tlTree: return _terrain.tree;
  case tlGrass: return (!_terrain.road && !_terrain.deepWater && !_terrain.water && !_terrain.tree && !_terrain.rock && _overlay.isNull());

  case isConstructible:
  {
    return !(_terrain.water || _terrain.deepWater || _terrain.rock || _terrain.tree || _overlay.isValid() || _terrain.road);
  }
  case tlMeadow: return _terrain.meadow;
  case tlRock: return _terrain.rock;
  case tlOverlay: return _overlay.isValid();
  case tlCoast: return _terrain.coast;
  case tlRubble: return _terrain.rubble;
  case isDestructible:
  {
    return _overlay.isValid()
              ? _overlay->isDestructible()
              : !(_terrain.rock || _terrain.water);
  }
  case tlGarden: return _terrain.garden;
  case tlElevation: return _terrain.elevation;
  case tlWall: return _terrain.wall;
  case wasDrawn: return _wasDrawn;
  case tlDeepWater: return _terrain.deepWater;
  default: break;
  }

  return false;
}

void Tile::setFlag(Tile::Type type, bool value)
{
  switch( type )
  {
  case tlRoad: _terrain.road = value; break;
  case tlWater: _terrain.water = value; break;
  case tlTree: _terrain.tree = value; break;
  case tlMeadow: _terrain.meadow = value; break;
  case tlRock: _terrain.rock = value; break;
  case tlCoast: _terrain.coast = value; break;
  case tlGarden: _terrain.garden = value; break;
  case tlElevation: _terrain.elevation = value; break;
  case tlRubble: _terrain.rubble = value; break;
  case clearAll: _terrain.clearFlags(); break;
  case tlWall: _terrain.wall = value; break;
  case wasDrawn: _wasDrawn = value; break;
  case tlDeepWater: _terrain.deepWater = value; break;
  default: break;
  }
}

TileOverlayPtr Tile::overlay() const{ return _overlay;}
void Tile::setOverlay(TileOverlayPtr overlay){  _overlay = overlay;}
unsigned int Tile::originalImgId() const{ return _terrain.imgid;}
void Tile::setOriginalImgId(unsigned short id){  _terrain.imgid = id;}
void Tile::setParam( Param param, int value) { _terrain.params[ param ] = value; }
void Tile::changeParam( Param param, int value) { _terrain.params[ param ] += value; }

int Tile::param( Param param) const
{
  std::map<Param, int>::const_iterator it = _terrain.params.find( param );
  return it != _terrain.params.end() ? it->second : 0;
}

}//end namespace gfx
