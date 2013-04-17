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

#include "oc3_water_buildings.hpp"

#include <iostream>

#include "oc3_scenario.hpp"
#include "oc3_time.hpp"
#include "oc3_positioni.hpp"
#include "oc3_resourcegroup.hpp"

namespace {
  static const char* rcAqueductGroup     = "land2a";
}

Aqueduct::Aqueduct()
{
  setType(B_AQUEDUCT);
  setPicture( PicLoader::instance().get_picture( rcAqueductGroup, 133) ); // default picture for aqueduct
  _size = 1;
  // land2a 119 120         - aqueduct over road
  // land2a 121 122         - aqueduct over plain ground
  // land2a 123 124 125 126 - aqueduct corner
  // land2a 127 128         - aqueduct over dirty roads
  // land2a 129 130 131 132 - aqueduct T-shape crossing
  // land2a 133             - aqueduct crossing
  // land2a 134 - 148       - aqueduct without water
}

Aqueduct* Aqueduct::clone() const
{
  return new Aqueduct(*this);
}

void Aqueduct::build(const int i, const int j)
{
  Construction::build(i, j);
  
  updateAqueducts(); // need to rewrite as computeAccessRoads()

  setPicture(computePicture());

  // update adjacent aqueducts
  if (_south != NULL) {_south->updateAqueducts(); if (dynamic_cast<Aqueduct*>(_south) != NULL) dynamic_cast<Aqueduct*>(_south)->setPicture(dynamic_cast<Aqueduct*>(_south)->computePicture());}
  if (_west  != NULL) {_west->updateAqueducts();  if (dynamic_cast<Aqueduct*>(_west) != NULL) dynamic_cast<Aqueduct*>(_west)->setPicture(dynamic_cast<Aqueduct*>(_west)->computePicture());}
  if (_north != NULL) {_north->updateAqueducts(); if (dynamic_cast<Aqueduct*>(_north) != NULL) dynamic_cast<Aqueduct*>(_north)->setPicture(dynamic_cast<Aqueduct*>(_north)->computePicture());}
  if (_east  != NULL) {_east->updateAqueducts();  if (dynamic_cast<Aqueduct*>(_east) != NULL) dynamic_cast<Aqueduct*>(_east)->setPicture(dynamic_cast<Aqueduct*>(_east)->computePicture());}
}

void Aqueduct::updateAqueducts()
{
  // TEMPORARY!!!!
  // find adjacent aqueducts
  int i = getTile().getI();
  int j = getTile().getJ();
  LandOverlay* __west   = Scenario::instance().getCity().getTilemap().at(i - 1, j).get_terrain().getOverlay();
  LandOverlay* __south  = Scenario::instance().getCity().getTilemap().at(i, j - 1).get_terrain().getOverlay();
  LandOverlay* __east   = Scenario::instance().getCity().getTilemap().at(i + 1, j).get_terrain().getOverlay();
  LandOverlay* __north  = Scenario::instance().getCity().getTilemap().at(i, j + 1).get_terrain().getOverlay();
  _south = dynamic_cast<WaterSource*>(__south); // if not aqueduct -> NULL
  _west  = dynamic_cast<WaterSource*>(__west);  // it is difficult to check against reservoir
  _north = dynamic_cast<WaterSource*>(__north); // because reservoir has four exits for water flow
  _east  = dynamic_cast<WaterSource*>(__east);  // and overlay is 3x3
}

void Aqueduct::setTerrain(TerrainTile &terrain)
{
  bool isRoad = false;
  isRoad = terrain.isRoad();
  terrain.reset();
  terrain.setOverlay(this);
  terrain.setBuilding(true);
  terrain.setRoad(isRoad);
  terrain.setAqueduct(true); // mandatory!
}

bool Aqueduct::canBuild(const int i, const int j) const
{
  bool is_free = Construction::canBuild(i, j);
  
  if (is_free) return true; // we try to build on free tile
  
  // we can place on road
  Tilemap& tilemap = Scenario::instance().getCity().getTilemap();

  // we can't build on plazas
  if (dynamic_cast<Plaza*>(tilemap.at(i, j).get_terrain().getOverlay()) != NULL)
    return false;

  // and we can't build on intersections
  if (tilemap.at(i, j).get_terrain().isRoad())
  {
    int directionFlags = 0;  // bit field, N=1, E=2, S=4, W=8
    if (tilemap.at(i, j + 1).get_terrain().isRoad()) { directionFlags += 1; } // road to the north
    if (tilemap.at(i, j - 1).get_terrain().isRoad()) { directionFlags += 4; } // road to the south
    if (tilemap.at(i + 1, j).get_terrain().isRoad()) { directionFlags += 2; } // road to the east
    if (tilemap.at(i - 1, j).get_terrain().isRoad()) { directionFlags += 8; } // road to the west

    std::cout << "direction flags=" << directionFlags << std::endl;
   
    int index;
    switch (directionFlags)
    {
    case 0:  // no road!
    case 1:  // North
    case 2:  // East
    case 4:  // South
    case 8:  // West
    case 5:  // North+South
    case 10: // East+West
      return true;
    }  
  }
  return false;
}

Picture& Aqueduct::computePicture()
{
  // find correct picture as for roads
   int i = getTile().getI();
   int j = getTile().getJ();
   
   int directionFlags = 0;  // bit field, N=1, E=2, S=4, W=8
   
   if (_north != NULL) { directionFlags += 1; }
   if (_east  != NULL) { directionFlags += 2; }
   if (_south != NULL) { directionFlags += 4; }
   if (_west  != NULL) { directionFlags += 8; }
   
   int index;
   switch (directionFlags)
   {
   case 0:  // no neighbours!
     index = 121; break;
   case 1:  // N
   case 4:  // S
   case 5:  // N + S
     index = 121; if (getTile().get_terrain().isRoad()) index = 119; break;
   case 3:  // N + E
     index = 123; break;
   case 6:  // E + S
     index = 124; break;
   case 7:  // N + E + S
     index = 129; break;
   case 9:  // N + W
     index = 126; break;
   case 2:  // E
   case 8:  // W
   case 10: // E + W
     index = 122; if (getTile().get_terrain().isRoad()) index = 120; break;
   case 11: // N + E + W
     index = 132; break;
   case 12: // S + W
     index = 125;  break;
   case 13: // N + S + W
     index = 131; break;
   case 14: // E + S + W
     index = 130; break;
   case 15: // N + S + E + W (crossing)
     index = 133; break;
   default:
     index = 121; // it's impossible, but ...
   }   
   
   return PicLoader::instance().get_picture( rcAqueductGroup, index);
}


Reservoir::Reservoir()
{
  setType(B_RESERVOIR);
  setPicture( PicLoader::instance().get_picture( ResourceGroup::utilitya, 34 ) );
 _size = 3;
  
  // utilitya 34      - emptry reservoir
  // utilitya 35 ~ 42 - full reservoir animation
 
  _animation.load( ResourceGroup::utilitya, 35, 8);
  _animation.load( ResourceGroup::utilitya, 42, 7, Animation::reverse);
  _animation.setFrameDelay( 11 );
  _animation.setOffset( Point( 47, 63 ) );
  _fgPictures.resize(1);
  //_fgPictures[0]=;
}

Reservoir* Reservoir::clone() const
{
  return new Reservoir(*this);
}

Reservoir::~Reservoir()
{
}

void Reservoir::build(const int i, const int j)
{
  Construction::build(i, j);  

  bool near_water = false;  // tells if the factory is next to a mountain

  Tilemap& tilemap = Scenario::instance().getCity().getTilemap();
  std::list<Tile*> rect = tilemap.getRectangle( i - 1, j - 1, i + _size, j + _size, false);
  for (std::list<Tile*>::iterator itTiles = rect.begin(); itTiles != rect.end(); ++itTiles)
  {
      near_water |= (*itTiles)->get_terrain().isWater();
  }

  _mayAnimate = near_water;
  
  updateAqueducts();
  
  // update adjacent aqueducts
  if (_south != NULL) {_south->updateAqueducts(); if (dynamic_cast<Aqueduct*>(_south) != NULL) dynamic_cast<Aqueduct*>(_south)->setPicture(dynamic_cast<Aqueduct*>(_south)->computePicture());}
  if (_west  != NULL) {_west->updateAqueducts();  if (dynamic_cast<Aqueduct*>(_west) != NULL) dynamic_cast<Aqueduct*>(_west)->setPicture(dynamic_cast<Aqueduct*>(_west)->computePicture());}
  if (_north != NULL) {_north->updateAqueducts(); if (dynamic_cast<Aqueduct*>(_north) != NULL) dynamic_cast<Aqueduct*>(_north)->setPicture(dynamic_cast<Aqueduct*>(_north)->computePicture());}
  if (_east  != NULL) {_east->updateAqueducts();  if (dynamic_cast<Aqueduct*>(_east) != NULL) dynamic_cast<Aqueduct*>(_east)->setPicture(dynamic_cast<Aqueduct*>(_east)->computePicture());}
}

void Reservoir::updateAqueducts()
{
  // TEMPORARY!!!! 
  // find adjacent aqueducts
  int i = getTile().getI();   
  int j = getTile().getJ();
  LandOverlay* __west   = Scenario::instance().getCity().getTilemap().at(i - 1, j + 1).get_terrain().getOverlay();
  LandOverlay* __south  = Scenario::instance().getCity().getTilemap().at(i + 1, j - 1).get_terrain().getOverlay();
  LandOverlay* __east   = Scenario::instance().getCity().getTilemap().at(i + 3, j + 1).get_terrain().getOverlay();
  LandOverlay* __north  = Scenario::instance().getCity().getTilemap().at(i + 1, j + 3).get_terrain().getOverlay();
  
  Reservoir* r_south = dynamic_cast<Reservoir*>(__south);
  Reservoir* r_west  = dynamic_cast<Reservoir*>(__west);
  Reservoir* r_east  = dynamic_cast<Reservoir*>(__east);
  Reservoir* r_north = dynamic_cast<Reservoir*>(__north);
    
  // it is very complex
  // need to simplify it
  
  if (r_south != NULL)
  {
    if (r_south->getTile().getI() == i && r_south->getTile().getJ() == j - 3)
      _south = dynamic_cast<WaterSource*>(__south);
  }
  else
    _south = dynamic_cast<WaterSource*>(__south); // if not aqueduct -> NULL

  if (r_west != NULL)
  {
    if (r_west->getTile().getI() == i - 3 && r_west->getTile().getJ() == j)
      _west = dynamic_cast<WaterSource*>(_west);    
  }
  else
    _west  = dynamic_cast<WaterSource*>(__west);  // it is difficult to check against reservoir
  if (r_north != NULL)
  {
    if (r_north->getTile().getI() == i && r_north->getTile().getJ() == j + 3)
      _north = dynamic_cast<WaterSource*>(_north);    
  }
  else
    _north = dynamic_cast<WaterSource*>(__north); // because reservoir has four exits for water flow
  if (r_east != NULL)
  {
    if (r_east->getTile().getI() == i + 3 && r_east->getTile().getJ() == j)
      _east = dynamic_cast<WaterSource*>(_east);    
  }
  else
    _east = dynamic_cast<WaterSource*>(__east);  // and overlay is 3x3
}

void Reservoir::setTerrain(TerrainTile &terrain)
{
  terrain.reset();
  terrain.setOverlay(this);
  terrain.setBuilding(true);
}

void Reservoir::timeStep(const unsigned long time)
{
    if( !_mayAnimate )
    {
        _fgPictures[ 0 ] = 0;
        return;
    }

    _animation.update( time );
    
    // takes current animation frame and put it into foreground
    _fgPictures[ 0 ] = _animation.getCurrentPicture(); 
}
