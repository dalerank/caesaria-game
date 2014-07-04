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

#include "tile.hpp"
#include "core/exception.hpp"
#include "objects/building.hpp"
#include "tileoverlay.hpp"
#include "game/resourcegroup.hpp"
#include "core/stringhelper.hpp"
#include "core/logger.hpp"
#include "game/gamedate.hpp"

namespace gfx
{

namespace {
  const int waterDecreaseInterval = GameDate::days2ticks( 15 );
  const int x_tileBase = 30;
  const int y_tileBase = x_tileBase / 2;
  Animation invalidAnimation;
}

void Tile::Terrain::reset()
{
  clearFlags();
  desirability = 0;
  watersrvc = 0;
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
  _epos = pos;
  _master = NULL;
  _wasDrawn = false;
  _overlay = NULL;
  _terrain.reset();
  _terrain.imgid = 0;
}

int Tile::i() const    {   return _pos.i();   }
int Tile::j() const    {   return _pos.j();   }
void Tile::setPicture(const Picture& picture) {  _picture = picture; }
void Tile::setPicture(const char* rc, const int index){ setPicture( Picture::load( rc, index ) );}
void Tile::setPicture(const std::string& name){ setPicture( Picture::load( name ) );}

const Picture& Tile::picture() const {  return _picture; }
Tile* Tile::masterTile() const{  return _master;}
void Tile::setMasterTile(Tile* master){  _master = master;}

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

const TilePos& Tile::pos() const{ return _pos; }
void Tile::setEPos(const TilePos& epos) { _epos = epos; }
Point Tile::center() const {  return Point( _pos.i(), _pos.j() ) * y_tileBase + Point( 7, 7); }
bool Tile::isMasterTile() const{  return (_master == this);}
Point Tile::mapPos() const{  return Point( x_tileBase * ( _epos.i() + _epos.j() ), y_tileBase * ( _epos.i() - _epos.j() ) );}

void Tile::animate(unsigned int time)
{
  if( _overlay.isNull() && _animation.isValid() )
  {
    _animation.update( time );
  }

  if( time % waterDecreaseInterval == 0)
  {
    decreaseWaterService( WTR_FONTAIN );
    decreaseWaterService( WTR_WELL );
  }
}

const Animation& Tile::animation() const{  return _overlay.isValid() ? invalidAnimation : _animation; }
void Tile::setAnimation(const Animation& animation){ _animation = animation;}

bool Tile::isWalkable( bool alllands ) const
{
  bool walkable = (_terrain.road || (alllands && !_terrain.water && !_terrain.tree && !_terrain.rock));
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
  case isConstructible:
  {
    return !(_terrain.water || _terrain.rock || _terrain.tree || _overlay.isValid() || _terrain.road);
  }
  case tlMeadow: return _terrain.meadow;
  case tlRock: return _terrain.rock;
  case tlBuilding: return _overlay.isValid();
  case tlCoast: return _terrain.coast;
  case tlRubble: return _terrain.rubble;
  case isDestructible:
  {
    return _overlay.isValid()
              ? _overlay->isDestructible()
              : _terrain.tree || _terrain.road;
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

void Tile::appendDesirability(int value){ _terrain.desirability += value; }
int Tile::desirability() const{  return _terrain.desirability;}
TileOverlayPtr Tile::overlay() const{ return _overlay;}
void Tile::setOverlay(TileOverlayPtr overlay){  _overlay = overlay;}
unsigned int Tile::originalImgId() const{ return _terrain.imgid;}
void Tile::setOriginalImgId(unsigned short id){  _terrain.imgid = id;}

void Tile::fillWaterService(WaterService type, int value)
{
  int vl = math::clamp( waterService( type )+value, 0, 0xf );
  _terrain.watersrvc |= ( vl << (type*4));
}

void Tile::decreaseWaterService(WaterService type, int value)
{
  int tmpSrvValue = math::clamp( waterService( type )-value, 0, 0xf);

  _terrain.watersrvc &= ~(0xf<<(type*4));
  _terrain.watersrvc |= tmpSrvValue << (type*4);
}

int Tile::waterService(WaterService type) const{  return (_terrain.watersrvc >> (type*4)) & 0xf;}

std::string TileHelper::convId2PicName( const unsigned int imgId )
{
  // example: for land1a_00004, pfx=land1a and id=4
  std::string res_pfx;  // resource name prefix
  int res_id = imgId;   // id of resource

  if( imgId < 245 )
  {
    res_pfx = ResourceGroup::plateau;
    res_id = imgId - 200;
  }
  else if( imgId < 548 )
  {
    res_pfx = ResourceGroup::land1a;
    res_id = imgId - 244;
  }
  else if( imgId < 779 )
  {
    res_pfx = ResourceGroup::land2a;
    res_id = imgId - 547;
  }
  else if( imgId < 871)
  {
    res_pfx = ResourceGroup::land3a;
    res_id = imgId - 778;
  }
  else
  {
    res_pfx = ResourceGroup::land1a;
    res_id = 1;

    // std::cout.setf(std::ios::hex, std::ios::basefield);
    // std::cout << "Unknown image Id " << imgId << std::endl;
    // std::cout.unsetf(std::ios::hex);

    if (imgId == 0xb10 || imgId == 0xb0d)
    {
      res_pfx = ResourceGroup::housing;
      res_id = 51;
    } // TERRIBLE HACK!

    // THROW("Unknown image Id " << imgId);
  }

  std::string ret_str = StringHelper::format( 0xff, "%s_%05d", res_pfx.c_str(), res_id );
  return ret_str;
}

int TileHelper::convPicName2Id( const std::string &pic_name )
{
  // example: for land1a_00004, return 244+4=248
  std::string res_pfx;  // resource name prefix = land1a
  int res_id = 0;   // idx of resource = 4

  // extract the name and idx from name (ex: [land1a, 4])
  int pos = pic_name.find("_");
  res_pfx = pic_name.substr(0, pos);
  std::stringstream ss(pic_name.substr(pos+1));
  ss >> res_id;

  if (res_pfx == ResourceGroup::plateau ){  res_id += 200; }
  else if (res_pfx == ResourceGroup::land1a) { res_id += 244; }
  else if (res_pfx == ResourceGroup::land2a) { res_id += 547; }
  else if (res_pfx == ResourceGroup::land3a) { res_id += 778; }
  else
  {
    Logger::warning( "TileHelper: unknown image " + pic_name );
  }

  return res_id;
}

int TileHelper::encode(const Tile& tt)
{
  int res = tt.getFlag( Tile::tlTree )   ? 0x00011 : 0;
  res += tt.getFlag( Tile::tlRock )      ? 0x00002 : 0;
  res += tt.getFlag( Tile::tlWater )     ? 0x00004 : 0;
  res += tt.getFlag( Tile::tlBuilding )  ? 0x00008 : 0;
  res += tt.getFlag( Tile::tlRoad )      ? 0x00040 : 0;
  res += tt.getFlag( Tile::tlCoast )     ? 0x00100 : 0;
  res += tt.getFlag( Tile::tlElevation ) ? 0x00200 : 0;
  res += tt.getFlag( Tile::tlMeadow )    ? 0x00800 : 0;
  res += tt.getFlag( Tile::tlRubble )    ? 0x01000 : 0;
  res += tt.getFlag( Tile::tlWall )      ? 0x04000 : 0;
  res += tt.getFlag( Tile::tlDeepWater ) ? 0x08000 : 0;
  res += tt.getFlag( Tile::tlRift )      ? 0x10000 : 0;
  return res;
}

unsigned int TileHelper::hash(const TilePos& pos)
{
  return (pos.i() << 16) + pos.j();
}

Point TileHelper::tilepos2screen(const TilePos& pos)
{
  return Point( 30 * (pos.i()+pos.j()), 15 * pos.z() );
}

void TileHelper::decode(Tile& tile, const int bitset)
{
  tile.setFlag( Tile::clearAll, true );

  if(bitset & 0x00001) { tile.setFlag( Tile::tlTree, true);      }
  if(bitset & 0x00002) { tile.setFlag( Tile::tlRock, true);      }
  if(bitset & 0x00004) { tile.setFlag( Tile::tlWater, true);     }
  //if(bitset & 0x8)   { tile.setFlag( Tile::tlBuilding, true);  }
  if(bitset & 0x00010) { tile.setFlag( Tile::tlTree, true);      }
  if(bitset & 0x00020) { tile.setFlag( Tile::tlGarden, true);    }
  if(bitset & 0x00040) { tile.setFlag( Tile::tlRoad, true);      }
  if(bitset & 0x00100) { tile.setFlag( Tile::tlCoast, true);     }
  if(bitset & 0x00200) { tile.setFlag( Tile::tlElevation, true); }
  if(bitset & 0x00400) { tile.setFlag( Tile::tlRock, true );     }
  if(bitset & 0x00800) { tile.setFlag( Tile::tlMeadow, true);    }
  if(bitset & 0x01000) { tile.setFlag( Tile::tlRubble, true);    }
  if(bitset & 0x04000) { tile.setFlag( Tile::tlWall, true);      }
  if(bitset & 0x08000) { tile.setFlag( Tile::tlDeepWater, true); }
  if(bitset & 0x10000) { tile.setFlag( Tile::tlRift, true);      }
}

Tile& TileHelper::getInvalid()
{
  static Tile invalidTile( TilePos( -1, -1) );
  return invalidTile;
}

}//end namespace gfx
