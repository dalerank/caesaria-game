// This file is part of openCaesar3.
//
// openCaesar3 is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// openCaesar3 is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with openCaesar3.  If not, see <http://www.gnu.org/licenses/>.

#include "oc3_tile.hpp"
#include "oc3_exception.hpp"
#include "oc3_building.hpp"
#include "oc3_resourcegroup.hpp"
#include "oc3_stringhelper.hpp"

Tile::Tile( const TilePos& pos) //: _terrain( 0, 0, 0, 0, 0, 0 )
{
  _pos = pos;
  _picture = NULL;
  _wasDrawn = false;
  _master = NULL;
}

Tile::Tile(const Tile& clone)
{
  _pos = clone._pos;
  _picture = clone._picture;
  _master = clone._master;
  _terrain = clone._terrain;
  _wasDrawn = clone._wasDrawn;
}

int Tile::getI() const    {   return _pos.getI();   }

int Tile::getJ() const    {   return _pos.getJ();   }


void Tile::setPicture(const Picture *picture)
{
  _picture = picture;
}

void Tile::setPicture(const char* rc, const int index)
{
  setPicture( &Picture::load( rc, index ) );
}

void Tile::setPicture(const std::string& name)
{
  setPicture( &Picture::load( name ) );
}

const Picture& Tile::getPicture() const
{
  _OC3_DEBUG_BREAK_IF( !_picture && "error: picture is null");

  return *_picture;
}

Tile* Tile::getMasterTile() const
{
  return _master;
}

void Tile::setMasterTile(Tile* master)
{
  _master = master;
}

bool Tile::isFlat() const
{
  return !(_terrain.isRock() || _terrain.isTree() || _terrain.isBuilding() || _terrain.isAqueduct());
}

TilePos Tile::getIJ() const
{
  return _pos;
}

bool Tile::isMasterTile() const
{
  return (_master == this);
}

Point Tile::getXY() const
{
  return Point( 30 * ( getI() + getJ()), 15 * (getI() - getJ()) );
}

void Tile::animate(unsigned int time)
{
  if( _terrain.getOverlay().isNull() && _animation.isValid() )
  {
    _animation.update( time );
  }
}

const Animation&Tile::getAnimation() const
{
  return _animation;
}

void Tile::setAnimation(const Animation& animation)
{
  _animation = animation;
}

bool Tile::isWalkable(bool alllands ) const
{
  return _terrain.isWalkable( alllands );
}

bool Tile::getFlag(Tile::Type type) const
{
  switch( type )
  {
  case tlRoad: return _terrain.isRoad();
  case tlWater: return _terrain.isWater();
  case tlTree: return _terrain.isTree();
  case isConstructible: return _terrain.isConstructible();
  case tlMeadow: return _terrain.isMeadow();
  case tlRock: return _terrain.isRock();
  case tlBuilding: return _terrain.isBuilding();
  case tlAqueduct: return _terrain.isAqueduct();
  case isDestructible: return _terrain.isDestructible();
  case tlGarden: return _terrain.isGarden();
  case tlElevation: return _terrain.isElevation();
  case tlWall: return _terrain.isWall();
  case tlGateHouse: return _terrain.isGateHouse();
  }

  return false;
}

void Tile::setFlag(Tile::Type type, bool value)
{
  switch( type )
  {
  case tlRoad: _terrain.setRoad( value ); break;
  case tlWater: _terrain.setWater( value ); break;
  case tlTree: _terrain.setTree( value ); break;
  case tlMeadow: _terrain.setMeadow( value ); break;
  case tlRock: _terrain.setRock( value ); break;
  case tlBuilding: _terrain.setBuilding( value ); break;
  case tlAqueduct: _terrain.setAqueduct( value ); break;
  case tlGarden: _terrain.setGarden( value ); break;
  case tlElevation: _terrain.setElevation( value ); break;
  case clearAll: _terrain.clearFlags(); break;
  case tlWall: _terrain.setWall( value ); break;
  case tlGateHouse: _terrain.setGateHouse( value ); break;
  }
}

void Tile::appendDesirability(int value)
{
  _terrain.appendDesirability( value );
}

int Tile::getDesirability() const
{
  return _terrain.getDesirability();
}

LandOverlayPtr Tile::getOverlay() const
{
  return _terrain.getOverlay();
}

void Tile::setOverlay(LandOverlayPtr overlay)
{
  _terrain.setOverlay( overlay );
}

unsigned int Tile::getOriginalImgId() const
{
  return _terrain.getOriginalImgId();
}

void Tile::setOriginalImgId(unsigned short id)
{
  _terrain.setOriginalImgId( id );
}

void Tile::fillWaterService(const WaterService type)
{
  _terrain.fillWaterService( type );
}

void Tile::decreaseWaterService(const WaterService type)
{
  _terrain.decreaseWaterService( type );
}

int Tile::getWaterService(const WaterService type) const
{
  return _terrain.getWaterService( type );
}

std::string TileHelper::convId2PicName( const unsigned int imgId )
{
  // example: for land1a_00004.png, pfx=land1a and id=4
  std::string res_pfx;  // resource name prefix
  int res_id = imgId;   // id of resource

  if( imgId < 245 )
  {
    res_pfx = "plateau";
    res_id = imgId - 200;
  }
  else if( imgId < 548 )
  {
    res_pfx = "land1a";
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
    res_pfx = "land1a";
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

  std::string ret_str = StringHelper::format( 0xff, "%s_%05d.png", res_pfx.c_str(), res_id );
  return ret_str;
}

int TileHelper::convPicName2Id( const std::string &pic_name )
{
  // example: for land1a_00004.png, return 244+4=248
  std::string res_pfx;  // resource name prefix = land1a
  int res_id;   // idx of resource = 4

  // extract the name and idx from name (ex: [land1a, 4])
  int pos = pic_name.find("_");
  res_pfx = pic_name.substr(0, pos);
  std::stringstream ss(pic_name.substr(pos+1));
  ss >> res_id;

  if (res_pfx == "plateau")
  {
    res_id += 200;
  }
  else if (res_pfx == "land1a")
  {
    res_id += 244;
  }
  else if (res_pfx == ResourceGroup::land2a)
  {
    res_id += 547;
  }
  else if (res_pfx == ResourceGroup::land3a)
  {
    res_id += 778;
  }
  else
  {
    THROW("Unknown image " << pic_name);
  }

  return res_id;
}

int TileHelper::encode(const Tile& tt)
{
  int res = tt.getFlag( Tile::tlTree ) ? 0x11 : 0;
  res += tt.getFlag( Tile::tlRock ) ? 0x2 : 0;
  res += tt.getFlag( Tile::tlWater ) ? 0x4 : 0;
  res += tt.getFlag( Tile::tlBuilding ) ? 0x8 : 0;
  res += tt.getFlag( Tile::tlRoad ) ? 0x40 : 0;
  res += tt.getFlag( Tile::tlMeadow ) ? 0x800 : 0;
  res += tt.getFlag( Tile::tlWall ) ? 0x4000 : 0;
  res += tt.getFlag( Tile::tlElevation ) ? 0x200 : 0;
  res += tt.getFlag( Tile::tlGateHouse ) ? 0x8000 : 0;
  return res;
}

void TileHelper::decode(Tile& tile, const int bitset)
{
  tile.setFlag( Tile::clearAll, true );

  if(bitset & 0x1)    { tile.setFlag( Tile::tlTree, true);      }
  if(bitset & 0x2)    { tile.setFlag( Tile::tlRock, true);      }
  if(bitset & 0x4)    { tile.setFlag( Tile::tlWater, true);     }
  if(bitset & 0x8)    { tile.setFlag( Tile::tlBuilding, true);  }
  if(bitset & 0x10)   { tile.setFlag( Tile::tlTree, true);      }
  if(bitset & 0x20)   { tile.setFlag( Tile::tlGarden, true);    }
  if(bitset & 0x40)   { tile.setFlag( Tile::tlRoad, true);      }
  if(bitset & 0x100)  { tile.setFlag( Tile::tlAqueduct, true);  }
  if(bitset & 0x200)  { tile.setFlag( Tile::tlElevation, true); }
  if(bitset & 0x400)  { tile.setFlag( Tile::tlRock, true );     }
  if(bitset & 0x800)  { tile.setFlag( Tile::tlMeadow, true);    }
  if(bitset & 0x4000) { tile.setFlag( Tile::tlWall, true);      }
  if(bitset & 0x8000) { tile.setFlag( Tile::tlGateHouse, true); }
}

