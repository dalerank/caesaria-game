#ifndef __OPENCAESAR3_TERRAININFO_H_INCLUDED__
#define __OPENCAESAR3_TERRAININFO_H_INCLUDED__

#include "oc3_serializer.hpp"

class LandOverlay;
class TilePos;

class TerrainTile : public Serializable
{
public:
  TerrainTile();

  // reset all fields
  void reset();

  bool isConstructible() const;
  bool isDestructible() const;
  bool isWalkable(const bool allLands) const;
  // isBurnable

  bool isWater() const                    { return _isWater;    }
  bool isRock() const                     { return _isRock;     }
  bool isTree() const                     { return _isTree;     }
  bool isBuilding() const                 { return _isBuilding; }
  bool isRoad() const                     { return _isRoad;     }
  bool isGarden() const                   { return _isGarden;   }
  bool isAqueduct() const                 { return _isAqueduct; }
  bool isMeadow() const                   { return _isMeadow;   }

  void setTree(const bool isTree)         { _isTree     = isTree;     }
  void setRock(const bool isRock)         { _isRock     = isRock;     }
  void setWater(const bool isWater)       { _isWater    = isWater;    }
  void setBuilding(const bool isBuilding) { _isBuilding = isBuilding; }
  void setGarden(const bool isGarden)     { _isGarden   = isGarden;   }
  void setRoad(const bool isRoad)         { _isRoad     = isRoad;     }
  void setAqueduct(const bool isAqueduct) { _isAqueduct = isAqueduct; }
  void setMeadow(const bool isMeadow)     { _isMeadow   = isMeadow;   }

  void setOverlay(LandOverlay *overlay);
  LandOverlay *getOverlay() const;

  // encode/decode to bitset
  int encode() const;
  void decode(const int bitset);

  void setOriginalImgId( unsigned int id );
  unsigned int getOriginalImgId() const;

  void serialize(OutputSerialStream &stream);
  void unserialize(InputSerialStream &stream);
private:
  bool _isWater;
  bool _isRock;
  bool _isTree;
  bool _isBuilding;
  bool _isRoad;
  bool _isGarden;
  bool _isAqueduct;
  bool _isMeadow;
  unsigned int _imgId;
  LandOverlay *_overlay;
};

class TerrainTileHelper
{
public:
  static std::string convId2PicName( const unsigned int imgId );
  static int convPicName2Id(std::string &pic_name);
};

#endif //__OPENCAESAR3_TERRAININFO_H_INCLUDED__