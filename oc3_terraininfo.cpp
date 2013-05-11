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

#include "oc3_terraininfo.hpp"
#include "oc3_building.hpp"
#include "oc3_exception.hpp"

#include <sstream>


TerrainTile::TerrainTile()
{
  reset();
}

void TerrainTile::reset()
{
  _desirability = 0;
  _isWater    = false;
  _isRock     = false;
  _isTree     = false;
  _isBuilding = false;
  _isRoad     = false;
  _isAqueduct = false;
  _isGarden   = false;
  _isMeadow   = false;
  _overlay    = NULL; // BUG? What will be with old overlay?
}

bool TerrainTile::isConstructible() const
{
  return !(_isWater || _isRock || _isTree || _isBuilding || _isRoad);
}

bool TerrainTile::isDestructible() const
{
  return (_isTree || _isBuilding || _isRoad);
}

bool TerrainTile::isWalkable(const bool allLand) const
{
  // TODO: test building to allow garden, gatehouse, granary, ...
  bool walkable = (_isRoad || (allLand && !_isWater && !_isTree && !_isRock));
  if( _overlay.isValid() )
  {
    walkable &= _overlay->isWalkable();
  }

  return walkable;
}

void TerrainTile::setOverlay(LandOverlayPtr overlay)
{
  _overlay = overlay;
}

LandOverlayPtr TerrainTile::getOverlay() const
{
  return _overlay;
}

int TerrainTile::encode() const
{
  int res = 0;
  if (isTree())
  {
    res += 0x1;
    res += 0x10;
  }
  if (isRock())     { res +=   0x2; }
  if (isWater())    { res +=   0x4; }
  if (isBuilding()) { res +=   0x8; }
  if (isRoad())     { res +=  0x40; }
  if (isMeadow())   { res += 0x800; }

  return res;
}

void TerrainTile::decode(const int bitset)
{
  reset();

  if (bitset & 0x1)    {  setTree(true);     }
  if (bitset & 0x2)    {  setRock(true);     }
  if (bitset & 0x4)    {  setWater(true);    }
  if (bitset & 0x8)    {  setBuilding(true); }
  if (bitset & 0x10)   {  setTree(true);     }
  if (bitset & 0x20)   {  setGarden(true);   }
  if (bitset & 0x40)   {  setRoad(true);     }
  if (bitset & 0x100)  {  setAqueduct(true); }
  if (bitset & 0x200)  {  setElevation(true);}
  if (bitset & 0x400)  {  setRock( true ); 
    //setAccessRamp(true);
  }
  if (bitset & 0x800)  {  setMeadow(true);   }
  //   if (bitset & 0x4000) {  setWall(true);   }
}

void TerrainTile::serialize(OutputSerialStream &stream)
{
  int terrainBitset = encode();
  stream.write_int(terrainBitset, 2, 0, 65535);
  stream.write_objectID( _overlay.object() );
}

void TerrainTile::unserialize(InputSerialStream &stream)
{
  int terrainBitset = stream.read_int(2, 0, 65535);
  decode(terrainBitset);
  //stream.read_objectID((void**)&_overlay.object());
}

void TerrainTile::appendDesirability( int value )
{
  _desirability = math::clamp( _desirability += value, -0xff, 0xff );
}

std::string TerrainTileHelper::convId2PicName( const unsigned int imgId )
{
  // example: for land1a_00004.png, pfx=land1a and id=4
  std::string res_pfx;  // resource name prefix
  int res_id = imgId;   // id of resource

  if (201<=imgId && imgId < 245)
  {
    res_pfx = "plateau";
    res_id = imgId - 200;
  }
  else if (245<=imgId && imgId < 548)
  {
    res_pfx = "land1a";
    res_id = imgId - 244;
  }
  else if (548<=imgId && imgId < 779)
  {
    res_pfx = "land2a";
    res_id = imgId - 547;
  }
  else if (779<=imgId && imgId < 871)
  {
    res_pfx = "land3a";
    res_id = imgId - 778;      
  }
  else
  {
    res_pfx = "land1a";
    res_id = 1;

    // std::cout.setf(std::ios::hex, std::ios::basefield);
    // std::cout << "Unknown image Id " << imgId << std::endl;
    // std::cout.unsetf(std::ios::hex);

    if (imgId == 0xb10 || imgId == 0xb0d) {res_pfx = "housng1a", res_id = 51;} // TERRIBLE HACK!

    // THROW("Unknown image Id " << imgId);
  }

  char ret_str[128];
  snprintf( ret_str, 127, "%s_%05d.png", res_pfx.c_str(), res_id );
  return std::string( ret_str );
}

int TerrainTileHelper::convPicName2Id( std::string &pic_name )
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
  else if (res_pfx == "land2a")
  {
    res_id += 547;
  }
  else if (res_pfx == "land3a")
  {
    res_id += 778;
  }
  else
  {
    THROW("Unknown image " << pic_name);
  }

  return res_id;
}