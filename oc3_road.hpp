#ifndef __OPENCAESAR3_ROAD_H_INCLUDE__
#define __OPENCAESAR3_ROAD_H_INCLUDE__

#include "oc3_building.hpp"

class Road : public Construction
{
public:
  Road();

  virtual Picture& computePicture();
  void updatePicture();

  void build(const TilePos& pos );
  void setTerrain(TerrainTile &terrain);
  bool canBuild(const TilePos& pos ) const;
  bool isWalkable() const;
};

class Plaza : public Road
{
public:
  Plaza();
  virtual void setTerrain(TerrainTile &terrain);  
  virtual bool canBuild(const TilePos& pos ) const;
  virtual Picture& computePicture();
};


#endif //__OPENCAESAR3_ROAD_H_INCLUDE__