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
#include "oc3_building_watersupply.hpp"
#include "oc3_city.hpp"
#include "oc3_tilemap.hpp"

Road::Road() : Construction( B_ROAD, Size(1) )
{
  _paved = 0;
}

void Road::build( CityPtr city, const TilePos& pos )
{
  Tilemap& tilemap = city->getTilemap();
  LandOverlayPtr overlay = tilemap.at( pos ).getOverlay();

  Construction::build( city, pos );
  setPicture( computePicture() );

  if( overlay.is<Road>() )
  {
    return;
  }

  if( overlay.is<Aqueduct>() )
  {
    overlay->build( city, pos );
    return;
  }

  // update adjacent roads
  /*foreach( Tile* tile, _accessRoads )
  {
    RoadPtr road = tile->getOverlay().as<Road>(); // let's think: may here different type screw up whole program?
    if( road.isValid() )
    {
      road->computeAccessRoads();
      road->setPicture(road->computePicture());
    }
  }*/
  
  // NOTE: also we need to update accessRoads for adjacent building
  // how to detect them if MaxDistance2Road can be any
  // so let's recompute accessRoads for every _building_
  /*LandOverlayList list = city->getOverlayList(); // it looks terrible!!!!
  foreach( LandOverlayPtr overlay, list )
  {
    BuildingPtr construction = overlay.as<Building>();
    if( construction.isValid() ) // if not valid then it isn't building
    {
      construction->computeAccessRoads();
    }
  }*/

  city->updateRoads();
}

bool Road::canBuild( CityPtr city, const TilePos& pos ) const
{
  bool is_free = Construction::canBuild( city, pos );

  if( is_free ) 
    return true; // we try to build on free tile

  LandOverlayPtr overlay  = city->getTilemap().at( pos ).getOverlay();

  return ( overlay.is<Aqueduct>() || overlay.is<Road>() );
}


void Road::initTerrain(Tile& terrain)
{
  terrain.setFlag( Tile::clearAll, true );
  terrain.setFlag( Tile::tlRoad, true );
}

Picture Road::computePicture()
{
  int i = getTile().getI();
  int j = getTile().getJ();

  TilemapTiles roads = getAccessRoads();
  int directionFlags = 0;  // bit field, N=1, E=2, S=4, W=8
  foreach( Tile* tile, roads )
  {
    if (tile->getJ() > j)      { directionFlags += 1; } // road to the north
    else if (tile->getJ() < j) { directionFlags += 4; } // road to the south
    else if (tile->getI() > i) { directionFlags += 2; } // road to the east
    else if (tile->getI() < i) { directionFlags += 8; } // road to the west
  }

  // std::cout << "direction flags=" << directionFlags << std::endl;

  int index;
  if( _paved == 0 )
  {
    switch (directionFlags)
    {
    case 0: index = 101; break; // no road!
    case 1: index = 101; break; // North
    case 2: index = 102; break; // East
    case 4: index = 103; break; // South
    case 8: index = 104; break; // West
    case 3: index = 97;  break; // North+East
    case 5: index = 93+2*(rand()%2); break;  // 93/95 // North+South
    case 6: index = 98;  break; // East+South
    case 7: index = 106; break; // North+East+South
    case 9: index = 100; break; // North+West
    case 10: index = 94+2*(rand()%2); break;  // 94/96 // East+West
    case 11: index = 109; break; // North+East+West
    case 12: index = 99; break;  // South+West
    case 13: index = 108; break; // North+South+West
    case 14: index = 107; break; // East+South+West
    case 15: index = 110; break; // North+East+South+West
    }
  }
  else
  {
    switch (directionFlags)
    {
    case 0: index = 52; break; // no road!
    case 1: index = 52+4*(rand()%2); break; // North
    case 2: index = 53; break; // East
    case 4: index = 54; break; // South
    case 8: index = 55; break; // West
    case 3: index = 48;  break; // North+East
    case 5: index = 44+2*(rand()%2); break;  // 93/95 // North+South
    case 6: index = 49;  break; // East+South
    case 9: index = 51; break; // North+West
    case 10: index = 45+2*(rand()%2); break;  // 94/96 // East+West
    case 12: index = 50; break;  // South+West

    case 7:
    case 11:
    case 13:
    case 14:
    case 15:
      index = 78 + rand() % 14;
    break;
    }
  }

  return Picture::load( ResourceGroup::road, index);
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

void Road::destroy()
{
  Construction::destroy();
}

void Road::appendPaved( int value )
{
  bool saveValue = _paved > 0;
  _paved = math::clamp( _paved += value, 0, 4 );

  if( saveValue != (_paved > 0) )
  {
    updatePicture();
  }
}

int Road::getPavedValue() const
{
  return _paved;
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


void Plaza::initTerrain(Tile& terrain)
{
  //std::cout << "Plaza::setTerrain" << std::endl;
  bool isMeadow = terrain.getFlag( Tile::tlMeadow );
  terrain.setFlag( Tile::clearAll , true);
  terrain.setFlag( Tile::tlRoad, true);
  terrain.setFlag( Tile::tlMeadow, isMeadow );
}

Picture Plaza::computePicture()
{
  //std::cout << "Plaza::computePicture" << std::endl;
  return Picture::load( ResourceGroup::entertaiment, 102);
}

// Plazas can be built ONLY on top of existing roads
// Also in original game there was a bug:
// gamer could place any number of plazas on one road tile (!!!)

bool Plaza::canBuild( CityPtr city, const TilePos& pos ) const
{
  //std::cout << "Plaza::canBuild" << std::endl;
  Tilemap& tilemap = city->getTilemap();

  bool is_constructible = true;

  TilemapArea area = tilemap.getArea( pos, getSize() ); // something very complex ???
  foreach( Tile* tile, area )
  {
    is_constructible &= tile->getFlag( Tile::tlRoad );
  }

  return is_constructible;
}


void Plaza::appendPaved(int value)
{

}
