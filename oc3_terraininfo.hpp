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

#ifndef __OPENCAESAR3_TERRAININFO_H_INCLUDED__
#define __OPENCAESAR3_TERRAININFO_H_INCLUDED__

#include <string>
#include "oc3_predefinitions.hpp"
#include "oc3_enums.hpp"
#include "oc3_flagholder.hpp"

class TilePos;

class TerrainTile
{
public:
  TerrainTile();
  TerrainTile(unsigned short int imgId, unsigned char edgeData,
	      unsigned short int terrainData, unsigned char terrainRandom,
	      unsigned char randomData, unsigned char elevationData);

  // reset all fields

  bool isConstructible() const;
  bool isDestructible() const;
  bool isWalkable(const bool allLands) const;
  // isBurnable

  bool isWater() const                    { return _isWater;     }
  bool isRock() const                     { return _isRock;      }
  bool isTree() const                     { return _isTree;      }
  bool isBuilding() const                 { return _isBuilding;  }
  bool isRoad() const                     { return _isRoad;      }
  bool isGarden() const                   { return _isGarden;    }
  bool isAqueduct() const                 { return _isAqueduct;  }
  bool isMeadow() const                   { return _isMeadow;    }
  bool isElevation() const                { return _isElevation; }
  bool isWall() const                     { return _isWall;      }
  bool isGateHouse() const                { return _isGateHouse; }

  void setTree(const bool isTree)           { _isTree      = isTree;      }
  void setRock(const bool isRock)           { _isRock      = isRock;      }
  void setWater(const bool isWater)         { _isWater     = isWater;     }
  void setBuilding(const bool isBuilding)   { _isBuilding  = isBuilding;  }
  void setGarden(const bool isGarden)       { _isGarden    = isGarden;    }
  void setRoad(const bool isRoad)           { _isRoad      = isRoad;      }
  void setAqueduct(const bool isAqueduct)   { _isAqueduct  = isAqueduct;  }
  void setMeadow(const bool isMeadow)       { _isMeadow    = isMeadow;    }
  void setElevation(const bool isElevation) { _isElevation = isElevation; }
  void setWall(const bool isWall)           { _isWall      = isWall;      }
  void setGateHouse(const bool isGateHouse) { _isGateHouse = isGateHouse; }
  void clearFlags();

  void setOverlay(LandOverlayPtr overlay);
  LandOverlayPtr getOverlay() const;

  // encode/decode to bitset
  int encode() const;
  void decode(const int bitset);

  void setOriginalImgId( unsigned short int id ) { _imgId = id;          }
 
  unsigned short int getOriginalImgId() const    { return _imgId;         }
  unsigned char      getEdgeData() const         { return _edgeData;      }
  unsigned short int getTerrainData() const      { return _terrainData;   }
  unsigned char      getTerrainRndmData() const  { return _terrainRandom; }
  unsigned char      getRandomData() const       { return _randomData;    }
  unsigned char      getElevationData() const    { return _elevationData; }
   
  int getDesirability() const                    { return _desirability; }
  void appendDesirability( int value );

  void fillWaterService( const WaterService type );
  void decreaseWaterService( const WaterService type );
  int getWaterService( const WaterService type ) const;

private:
  void _reset();

  bool _isWater;
  bool _isRock;
  bool _isTree;
  bool _isBuilding;
  bool _isRoad;
  bool _isGarden;
  bool _isAqueduct;
  bool _isMeadow;
  bool _isElevation;
  bool _isWall;
  bool _isGateHouse;
  int  _desirability;
  int  _waterService;
  
  /*
   * original tile information
   */
  unsigned short int _imgId;      
  unsigned char      _edgeData;
  unsigned short int _terrainData;
  unsigned char      _terrainRandom;
  unsigned char      _randomData;
  unsigned char      _elevationData;
    
  LandOverlayPtr _overlay;
};

class TerrainTileHelper
{
public:
  static std::string convId2PicName( const unsigned int imgId );
  static int convPicName2Id( const std::string &pic_name);
};

#endif //__OPENCAESAR3_TERRAININFO_H_INCLUDED__