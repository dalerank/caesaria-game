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

#include "oc3_road.hpp"
#include "oc3_tile.hpp"
#include "oc3_resourcegroup.hpp"
#include "oc3_scenario.hpp"
#include "oc3_water_buildings.hpp"

Road::Road() : Construction( B_ROAD, Size(1) )
{
  setPicture( Picture::load( ResourceGroup::road, 44));  // default picture for build tool
}

void Road::build(const TilePos& pos )
{
  Tilemap& tilemap = Scenario::instance().getCity().getTilemap();
  LandOverlayPtr overlay = tilemap.at( pos ).getTerrain().getOverlay();

  Construction::build( pos );
  setPicture( computePicture() );

  if( overlay.is<Road>() )
  {
    return;
  }

  if( overlay.is<Aqueduct>() )
  {
    overlay->build( pos );
    return;
  }

  // update adjacent roads
  for( PtrTilesList::iterator itTile = _accessRoads.begin(); itTile != _accessRoads.end(); ++itTile)
  {
    RoadPtr road = (*itTile)->getTerrain().getOverlay().as<Road>(); // let's think: may here different type screw up whole program?
    if( road.isValid() )
    {
      road->computeAccessRoads();
      road->setPicture(road->computePicture());
    }
  }
  
  // NOTE: also we need to update accessRoads for adjacent building
  // how to detect them if MaxDistance2Road can be any
  // so let's recompute accessRoads for every _building_
  LandOverlays list = Scenario::instance().getCity().getOverlayList(); // it looks terrible!!!!
  for( LandOverlays::iterator itOverlay = list.begin(); itOverlay!=list.end(); ++itOverlay )
  {
    BuildingPtr construction = (*itOverlay).as<Building>();
    if( construction.isValid() ) // if not valid then it isn't building
    {
      construction->computeAccessRoads();
    }
  }
}

bool Road::canBuild(const TilePos& pos ) const
{
  bool is_free = Construction::canBuild( pos );

  if( is_free ) 
    return true; // we try to build on free tile

  Tilemap& tilemap = Scenario::instance().getCity().getTilemap();
  TerrainTile& terrain = tilemap.at( pos ).getTerrain();

  return ( terrain.getOverlay().is<Aqueduct>() || terrain.getOverlay().is<Road>() );
}


void Road::setTerrain(TerrainTile& terrain)
{
  terrain.clearFlags();
  terrain.setOverlay( this );
  terrain.setRoad( true );
}

Picture& Road::computePicture()
{
  int i = getTile().getI();
  int j = getTile().getJ();

  PtrTilesList roads = getAccessRoads();
  int directionFlags = 0;  // bit field, N=1, E=2, S=4, W=8
  for( PtrTilesList::iterator itRoads = roads.begin(); itRoads!=roads.end(); ++itRoads)
  {
    Tile* tile = *itRoads;
    if (tile->getJ() > j)      { directionFlags += 1; } // road to the north
    else if (tile->getJ() < j) { directionFlags += 4; } // road to the south
    else if (tile->getI() > i) { directionFlags += 2; } // road to the east
    else if (tile->getI() < i) { directionFlags += 8; } // road to the west
  }

  // std::cout << "direction flags=" << directionFlags << std::endl;

  int index;
  switch (directionFlags)
  {
  case 0:  // no road!
    index = 101;
    break;
  case 1:  // North
    index = 101;
    break;
  case 2:  // East
    index = 102;
    break;
  case 4:  // South
    index = 103;
    break;
  case 8:  // West
    index = 104;
    break;
  case 3:  // North+East
    index = 97;
    break;
  case 5:  // North+South
    index = 93+2*(rand()%2);
    break;  // 93/95
  case 6:  // East+South
    index = 98;
    break;
  case 7:  // North+East+South
    index = 106;
    break;
  case 9:  // North+West
    index = 100;
    break;
  case 10:  // East+West
    index = 94+2*(rand()%2);
    break;  // 94/96
  case 11:  // North+East+West
    index = 109;
    break;
  case 12:  // South+West
    index = 99;
    break;
  case 13:  // North+South+West
    index = 108;
    break;
  case 14:  // East+South+West
    index = 107;
    break;
  case 15:  // North+East+South+West
    index = 110;
    break;
  }

  Picture *picture = &Picture::load( ResourceGroup::road, index);
  return *picture;
}

bool Road::isWalkable() const
{
  return true;
}

void Road::updatePicture()
{
  setPicture( computePicture() );
}

bool Road::isNeedRoadAccess() const
{
  return false;
}
// I didn't decide what is the best approach: make Plaza as constructions or as upgrade to roads
Plaza::Plaza()
{
  // somewhere we need to delete original road and then we need to think
  // because as we remove original road we need to recompute adjacent tiles
  // or we will run into big troubles

  setType(B_PLAZA);
  setPicture(computePicture()); // 102 ~ 107
  setSize( Size( 1 ) );
}


void Plaza::setTerrain(TerrainTile& terrain)
{
  //std::cout << "Plaza::setTerrain" << std::endl;
  bool isMeadow = terrain.isMeadow();  
  terrain.clearFlags();
  terrain.setOverlay(this);
  terrain.setRoad(true);
  terrain.setMeadow( isMeadow );
}

Picture& Plaza::computePicture()
{
  //std::cout << "Plaza::computePicture" << std::endl;
  return Picture::load( ResourceGroup::entertaiment, 102);
}

// Plazas can be built ONLY on top of existing roads
// Also in original game there was a bug:
// gamer could place any number of plazas on one road tile (!!!)

bool Plaza::canBuild(const TilePos& pos ) const
{
  //std::cout << "Plaza::canBuild" << std::endl;
  Tilemap& tilemap = Scenario::instance().getCity().getTilemap();

  bool is_constructible = true;

  PtrTilesArea rect = tilemap.getFilledRectangle( pos, getSize() ); // something very complex ???
  for( PtrTilesArea::iterator itTiles = rect.begin(); itTiles != rect.end(); ++itTiles)
  {
    is_constructible &= (*itTiles)->getTerrain().isRoad();
  }

  return is_constructible;
}
