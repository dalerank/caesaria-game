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
#include "oc3_variant.hpp"
#include "oc3_stringhelper.hpp"
#include "oc3_resourcegroup.hpp"

TerrainTile::TerrainTile()
{
  _reset();
  _imgId = 0;
}

/*TerrainTile::TerrainTile(unsigned short int imgId, unsigned char edgeData,
	      unsigned short int terrainData, unsigned char terrainRandom,
	      unsigned char randomData, unsigned char elevationData) :
	      _imgId(imgId), _edgeData(edgeData), _terrainData(terrainData), 
	      _terrainRandom(terrainRandom), _randomData(randomData), _elevationData(elevationData)
{
  _reset();
  //decode(terrainData);
}*/

void TerrainTile::_reset()
{
  clearFlags();
  _desirability = 0;
  _waterService = 0;
  _overlay      = NULL; // BUG? What will be with old overlay?
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

void TerrainTile::appendDesirability( int value )
{
  _desirability = math::clamp( _desirability += value, -0xff, 0xff );
}

void TerrainTile::fillWaterService( const WaterService type )
{
  _waterService |= (0xf << (type*4));
}

void TerrainTile::decreaseWaterService( const WaterService type )
{
  int tmpSrvValue = (_waterService >> (type*4)) & 0xf;
  //tmpSrvValue = math::clamp( tmpSrvValue-1, 0, 0xf );
  tmpSrvValue = 0;

  _waterService &= ~(0xf<<(type*4));
  _waterService |= tmpSrvValue << (type*4);
}

int TerrainTile::getWaterService( const WaterService type ) const
{
  return (_waterService >> (type*4)) & 0xf;
}

void TerrainTile::clearFlags()
{
  _isWater      = false;
  _isRock       = false;
  _isTree       = false;
  _isBuilding   = false;
  _isRoad       = false;
  _isAqueduct   = false;
  _isGarden     = false;
  _isMeadow     = false;
  _isWall       = false;
  _isGateHouse  = false;
}
