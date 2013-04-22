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
#include <stdexcept>

#include "oc3_scenario.hpp"
#include "oc3_time.hpp"
#include "oc3_positioni.hpp"
#include "oc3_resourcegroup.hpp"
#include "oc3_safetycast.hpp"

Aqueduct::Aqueduct()
{
  setType(B_AQUEDUCT);
  setPicture( Picture::load( ResourceGroup::aqueduct, 133) ); // default picture for aqueduct
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

void Aqueduct::build(const TilePos& pos )
{
  Tilemap& tilemap = Scenario::instance().getCity().getTilemap();
  TerrainTile& terrain = tilemap.at( pos ).get_terrain();

  // we can't build on plazas
  if( dynamic_cast<Aqueduct*>( terrain.getOverlay() ) != 0 )
        return;

  Construction::build( pos );
  
  updateAqueducts(); // need to rewrite as computeAccessRoads()
}

void Aqueduct::link(Aqueduct& target)
{
  // check target coordinates and compare with our coords
  int i = getTile().getI();
  int j = getTile().getJ();
  
  int oppi = target.getTile().getI();
  int oppj = target.getTile().getJ();
  
  if ((i == oppi - 1) && (j == oppj))      { _east = &target;  target._west = this;}
  else if ((i == oppi) && (j == oppj - 1)) { _north = &target; target._south = this;}
  else if ((i == oppi + 1) && (j == oppj)) { _west = &target;  target._east = this;}
  else if ((i == oppi) && (j == oppj + 1)) { _south = &target; target._north = this;}
  
  setPicture(computePicture());
}

void Aqueduct::link(Reservoir& target)
{
  // check target coordinates and compare with our coords
  int i = getTile().getI();
  int j = getTile().getJ();
  
  int oppi = target.getTile().getI();
  int oppj = target.getTile().getJ();
  
  // when we get Reservoir coords, we get master tile coords
  // so try to calculate center of Reservoir (it will be easier after)
  oppi++; oppj++;
  
  if ((i == oppi - 2) && (j == oppj))      { _east = &target;  target._west = this;}
  else if ((i == oppi) && (j == oppj - 2)) { _north = &target; target._south = this;}
  else if ((i == oppi + 2) && (j == oppj)) { _west = &target;  target._east = this;}
  else if ((i == oppi) && (j == oppj + 2)) { _south = &target; target._north = this;}
  
  setPicture(computePicture());
}

void Aqueduct::destroy()
{
//  std::cout << "Aqueduct::destroy()" << std::endl;
  
  // update adjacent aqueducts

  if (_south != NULL) { _south->_north = NULL; Aqueduct *a = dynamic_cast<Aqueduct*>(_south); if (a!=NULL) a->setPicture(a->computePicture());}
  if (_west  != NULL) { _west->_east = NULL;   Aqueduct *a = dynamic_cast<Aqueduct*>(_west);  if (a!=NULL) a->setPicture(a->computePicture());}
  if (_north != NULL) { _north->_south = NULL; Aqueduct *a = dynamic_cast<Aqueduct*>(_north); if (a!=NULL) a->setPicture(a->computePicture());}
  if (_east  != NULL) { _east->_west = NULL;   Aqueduct *a = dynamic_cast<Aqueduct*>(_east);  if (a!=NULL) a->setPicture(a->computePicture());}

  Construction::destroy();
}

void Reservoir::destroy()
{
//  std::cout << "Reservoir::destroy()" << std::endl;
  
  // update adjacent aqueducts

  if (_south != NULL) { _south->_north = NULL; dynamic_cast<Aqueduct*>(_south)->setPicture(dynamic_cast<Aqueduct*>(_south)->computePicture());}
  if (_west  != NULL) { _west->_east = NULL;   dynamic_cast<Aqueduct*>(_west)->setPicture(dynamic_cast<Aqueduct*>(_west)->computePicture());}
  if (_north != NULL) { _north->_south = NULL; dynamic_cast<Aqueduct*>(_north)->setPicture(dynamic_cast<Aqueduct*>(_north)->computePicture());}
  if (_east  != NULL) { _east->_west = NULL;   dynamic_cast<Aqueduct*>(_east)->setPicture(dynamic_cast<Aqueduct*>(_east)->computePicture());}

  Construction::destroy();
}


void Reservoir::link(Aqueduct& target)
{
  // check target coordinates and compare with our coords
  int i = getTile().getI() + 1;
  int j = getTile().getJ() + 1;
  
  int oppi = target.getTile().getI();
  int oppj = target.getTile().getJ();

  if ((i == oppi - 2) && (j == oppj))      { _east = &target;  target._west = this;}
  else if ((i == oppi) && (j == oppj - 2)) { _north = &target; target._south = this;}
  else if ((i == oppi + 2) && (j == oppj)) { _west = &target;  target._east = this;}
  else if ((i == oppi) && (j == oppj + 2)) { _south = &target; target._north = this;}  
}

void Reservoir::link(Reservoir& target)
{
  // nothing to do
  // reservoirs can't be connected to each other
}

void Aqueduct::updateAqueducts()
{
  int i = getTile().getI();
  int j = getTile().getJ();
  
  std::list<Tile*> rect = Scenario::instance().getCity().getTilemap().getRectangle( TilePos( i - 1, j - 1),
                                                TilePos( i + 1, j + 1), 
                                                !Tilemap::checkCorners );
  for (std::list<Tile*>::iterator itTiles = rect.begin(); itTiles != rect.end(); ++itTiles)
  {
    Tile* tile = *itTiles;
    WaterSource* ws = dynamic_cast<WaterSource*>(tile->get_terrain().getOverlay());
    if (ws != NULL) ws->link(*this);
  }  
  
  setPicture(computePicture());
}

void Reservoir::updateAqueducts()
{
  // TEMPORARY!!!!
  // find adjacent aqueducts
  int i = getTile().getI() + 1;
  int j = getTile().getJ() + 1;
  
  try
  {
  WaterSource* __west   = dynamic_cast<WaterSource*>(Scenario::instance().getCity().getTilemap().at(i - 2, j).get_terrain().getOverlay());
  if (__west  != NULL) __west->link(*this);
  }
  catch(std::out_of_range)
  {
    
  }

  
  try
  {
  WaterSource* __south  = dynamic_cast<WaterSource*>(Scenario::instance().getCity().getTilemap().at(i, j - 2).get_terrain().getOverlay());
  if (__south != NULL) __south->link(*this);
  }
  catch(std::out_of_range)
  {
    
  }

  
  try
  {
  WaterSource* __east   = dynamic_cast<WaterSource*>(Scenario::instance().getCity().getTilemap().at(i + 2, j).get_terrain().getOverlay());
  if (__east  != NULL) __east->link(*this);
  }
  catch(std::out_of_range)
  {
    
  }

  
  try
  {
  WaterSource* __north  = dynamic_cast<WaterSource*>(Scenario::instance().getCity().getTilemap().at(i, j + 2).get_terrain().getOverlay());
  if (__north != NULL) __north->link(*this);
  }
  catch(std::out_of_range)
  {
    
  }
  
  
  
}

void Aqueduct::setTerrain(TerrainTile &terrain)
{
  bool isRoad   = terrain.isRoad();
  bool isMeadow = terrain.isMeadow();
  terrain.reset();
  terrain.setOverlay(this);
  terrain.setBuilding(true);
  terrain.setRoad(isRoad);
  terrain.setMeadow(isMeadow);
  terrain.setAqueduct(true); // mandatory!
}

bool Aqueduct::canBuild( const TilePos& pos ) const
{
  bool is_free = Construction::canBuild( pos );
  
  if( is_free ) 
      return true; // we try to build on free tile
  
  // we can place on road
  Tilemap& tilemap = Scenario::instance().getCity().getTilemap();
  TerrainTile& terrain = tilemap.at( pos ).get_terrain();

  // we can't build on plazas
  if( dynamic_cast<Plaza*>( terrain.getOverlay() ) != NULL)
      return false;

  // we can show that won't build over other aqueduct
  if( safety_cast< Aqueduct* >( terrain.getOverlay() ) != NULL )
      return false;

  // also we can't build if next tile is road + aqueduct
  if ( terrain.isRoad() )
  {
    std::list<Tile*> rect = Scenario::instance().getCity().getTilemap().getRectangle( pos + TilePos (-1, -1),
                                                pos + TilePos (1, 1), 
                                                !Tilemap::checkCorners );
    for (std::list<Tile*>::iterator itTiles = rect.begin(); itTiles != rect.end(); ++itTiles)
    {
      Tile* tile = *itTiles;
      if (tile->get_terrain().isRoad() && tile->get_terrain().isAqueduct())
      {
	      return false;
      }
    }    
  }
  
  // and we can't build on intersections
  if ( terrain.isRoad() )
  {
    int directionFlags = 0;  // bit field, N=1, E=2, S=4, W=8
    if (tilemap.at( pos + TilePos(  0,  1 ) ).get_terrain().isRoad()) { directionFlags += 1; } // road to the north
    if (tilemap.at( pos + TilePos(  0, -1 ) ).get_terrain().isRoad()) { directionFlags += 4; } // road to the south
    if (tilemap.at( pos + TilePos(  1,  0 ) ).get_terrain().isRoad()) { directionFlags += 2; } // road to the east
    if (tilemap.at( pos + TilePos( -1,  0 ) ).get_terrain().isRoad()) { directionFlags += 8; } // road to the west

    std::cout << "direction flags=" << directionFlags << std::endl;
   
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
   
   return Picture::load( ResourceGroup::aqueduct, index);
}

void Aqueduct::updatePicture()
{
  setPicture(computePicture());
}

Reservoir::Reservoir()
{
  setType(B_RESERVOIR);
  setPicture( Picture::load( ResourceGroup::waterbuildings, 1 )  );
  _size = 3;
  
  // utilitya 34      - empty reservoir
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

void Reservoir::build(const TilePos& pos )
{
  Construction::build( pos );  

  setPicture( Picture::load( ResourceGroup::waterbuildings, 1 ) );
  _mayAnimate = _isNearWater( pos );
  
  updateAqueducts();
  
  // update adjacent aqueducts
  if (dynamic_cast<Aqueduct*>(_south) != NULL) dynamic_cast<Aqueduct*>(_south)->setPicture(dynamic_cast<Aqueduct*>(_south)->computePicture());
  if (dynamic_cast<Aqueduct*>(_west) != NULL) dynamic_cast<Aqueduct*>(_west)->setPicture(dynamic_cast<Aqueduct*>(_west)->computePicture());
  if (dynamic_cast<Aqueduct*>(_north) != NULL) dynamic_cast<Aqueduct*>(_north)->setPicture(dynamic_cast<Aqueduct*>(_north)->computePicture());
  if (dynamic_cast<Aqueduct*>(_east) != NULL) dynamic_cast<Aqueduct*>(_east)->setPicture(dynamic_cast<Aqueduct*>(_east)->computePicture());
}

bool Reservoir::_isNearWater( const TilePos& pos ) const
{
  bool near_water = false;  // tells if the factory is next to a mountain

  Tilemap& tilemap = Scenario::instance().getCity().getTilemap();
  std::list<Tile*> rect = tilemap.getRectangle( pos + TilePos( -1, -1 ), Size( _size+1 ), !Tilemap::checkCorners );
  for (std::list<Tile*>::iterator itTiles = rect.begin(); itTiles != rect.end(); ++itTiles)
  {
    near_water |= (*itTiles)->get_terrain().isWater();
  }

  return near_water;
}

void Reservoir::setTerrain(TerrainTile &terrain)
{
  bool isMeadow = terrain.isMeadow();
  terrain.reset();
  terrain.setOverlay(this);
  terrain.setBuilding(true);
  terrain.setMeadow(isMeadow);
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

bool Reservoir::canBuild( const TilePos& pos ) const
{
  bool ret = Construction::canBuild( pos );

  bool nearWater = _isNearWater( pos );
  const_cast< Reservoir* >( this )->setPicture( Picture::load( ResourceGroup::waterbuildings, nearWater ? 2 : 1 )  );

  return ret;
}