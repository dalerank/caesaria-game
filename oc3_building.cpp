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
//
// Copyright 2012-2013 Gregoire Athanase, gathanase@gmail.com



#include "oc3_building.hpp"

#include <iostream>
#include <algorithm>

#include "oc3_scenario.hpp"
#include "oc3_walker.hpp"
#include "oc3_exception.hpp"
#include "oc3_gui_info_box.hpp"
#include "oc3_building_data.hpp"
#include "oc3_factory_building.hpp"
#include "oc3_service_building.hpp"
#include "oc3_training_building.hpp"
#include "oc3_warehouse.hpp"
#include "oc3_gettext.hpp"
#include "oc3_sdl_facade.hpp"
#include "oc3_time.hpp"
#include "oc3_burningruins.hpp"
#include "oc3_collapsedruins.hpp"
#include "oc3_water_buildings.hpp"
#include "oc3_constructionmanager.hpp"
#include "oc3_resourcegroup.hpp"

namespace {
static const char* rcRoadGroup         = "land2a";
static const char* rcHousingGroup      = "housng1a";
static const char* rcGovernmentGroup   = "govt";
static const char* rcEntertaimentGroup = "entertainment";
}

//std::map<BuildingType, LandOverlay*> LandOverlay::_mapBuildingByID;  // key=buildingType, value=instance

LandOverlay::LandOverlay(const BuildingType type, const Size& size)
{
   _master_tile = NULL;
   _size = size.getWidth();
   _isDeleted = false;
   _name = "unknown";
   _picture = NULL;
   setType( type );
}

LandOverlay::~LandOverlay()
{
  // what we shall to do here?
  int i=0;
}


BuildingType LandOverlay::getType() const
{
   return _buildingType;
}

void LandOverlay::setType(const BuildingType buildingType)
{
   _buildingType = buildingType;
   _name = BuildingDataHolder::instance().getData(buildingType).getName();
}

void LandOverlay::timeStep(const unsigned long time) { }

void LandOverlay::setPicture(Picture &picture)
{
   Tilemap &tilemap = Scenario::instance().getCity().getTilemap();

   _picture = &picture;

   if (_master_tile != NULL)
   {
      // _master_tile == NULL is cloneable buildings
      int i = _master_tile->getI();
      int j = _master_tile->getJ();

      for (int dj = 0; dj<_size; ++dj)
      {
         for (int di = 0; di<_size; ++di)
         {
            Tile &tile = tilemap.at(i+di, j+dj);
            tile.set_picture(_picture);
         }
      }
   }
}

void LandOverlay::build( const TilePos& pos )
{
    City &city = Scenario::instance().getCity();
    Tilemap &tilemap = city.getTilemap();

    _master_tile = &tilemap.at( pos );

    for (int dj = 0; dj<_size; ++dj)
    {
        for (int di = 0; di<_size; ++di)
        {
            Tile& tile = tilemap.at( pos + TilePos( di, dj ) );
            tile.set_master_tile(_master_tile);
            tile.set_picture(_picture);
            TerrainTile& terrain = tile.get_terrain();
            terrain.setOverlay(this);
            setTerrain( terrain );
        }
    }
}

void LandOverlay::deleteLater()
{
  _isDeleted  = true;
}

void LandOverlay::destroy()
{
  int i=0;
}

Tile& LandOverlay::getTile() const
{
  _OC3_DEBUG_BREAK_IF( !_master_tile && "master tile must be exists" );
  return *_master_tile;
}

int LandOverlay::getSize() const
{
  return _size;
}

bool LandOverlay::isDeleted() const
{
  return _isDeleted;
}

Picture& LandOverlay::getPicture()
{
  if (_picture == NULL)
  {
     THROW("Picture is NULL!");
  }
  return *_picture;
}

std::vector<Picture*>& LandOverlay::getForegroundPictures()
{
  return _fgPictures;
}


GuiInfoBox* LandOverlay::makeInfoBox()
{
  std::cout << "LandOverlay::makeInfoBox()" << std::endl;
  return NULL;
}

std::string LandOverlay::getName()
{
  return _name;
}

void LandOverlay::serialize(OutputSerialStream &stream)
{
  stream.write_objectID(this);
  stream.write_int((int) _buildingType, 1, 0, B_MAX);
  stream.write_int(getTile().getI(), 2, 0, 1000);
  stream.write_int(getTile().getJ(), 2, 0, 1000);
}

LandOverlay& LandOverlay::unserialize_all(InputSerialStream &stream)
{
  int objectID = stream.read_objectID();
  BuildingType buildingType = (BuildingType) stream.read_int(1, 0, B_MAX);
  int i = stream.read_int(2, 0, 1000);
  int j = stream.read_int(2, 0, 1000);
  LandOverlay *res = ConstructionManager::getInstance().create( buildingType );
  res->_master_tile = &Scenario::instance().getCity().getTilemap().at(i, j);
  res->unserialize(stream);
  stream.link(objectID, res);
  return *res;
}

void LandOverlay::unserialize(InputSerialStream &stream)
{
}

bool LandOverlay::isWalkable() const
{
  return false;
}

TilePos LandOverlay::getTilePos() const
{
  return _master_tile->getIJ();
}

Construction::Construction( const BuildingType type, const Size& size)
: LandOverlay( type, size )
{
}

bool Construction::canBuild( const TilePos& pos ) const
{
  Tilemap& tilemap = Scenario::instance().getCity().getTilemap();

  bool is_constructible = true;

  std::list<Tile*> rect = tilemap.getFilledRectangle( pos, Size( _size ) );
  for (std::list<Tile*>::iterator itTiles = rect.begin(); itTiles != rect.end(); ++itTiles)
  {
     is_constructible &= (*itTiles)->get_terrain().isConstructible();
  }

  return is_constructible;
}

void Construction::build(const TilePos& pos )
{
  LandOverlay::build( pos );
  computeAccessRoads();
  _updateDesirabilityInfluence( true );
}

void Construction::_updateDesirabilityInfluence( bool onBuild )
{
  City &city = Scenario::instance().getCity();
  Tilemap &tilemap = city.getTilemap();

  PtrTilesList desirabilityArea = tilemap.getFilledRectangle( getTile().getIJ() - TilePos(2, 2), 
    getTile().getIJ() + TilePos( 2, 2 ) );

  int mul = (onBuild ? 1 : -1);
  for( PtrTilesList::iterator it=desirabilityArea.begin(); it != desirabilityArea.end(); it++ )
  {
    (*it)->get_terrain().appendDesirability( mul * getDesirabilityInfluence() );
  }
}


const PtrTilesList& Construction::getAccessRoads() const
{
   return _accessRoads;
}

// here the problem lays: if we remove road, it is left in _accessRoads array
// also we need to recompute _accessRoads if we place new road tile
// on next to this road tile buildings
void Construction::computeAccessRoads()
{
  _accessRoads.clear();
  if( !_master_tile )
      return;

  Tilemap& tilemap = Scenario::instance().getCity().getTilemap();

  Uint8 maxDst2road = getMaxDistance2Road();
  std::list<Tile*> rect = tilemap.getRectangle( _master_tile->getIJ() + TilePos( -maxDst2road, -maxDst2road ),
                                                _master_tile->getIJ() + TilePos( _size + maxDst2road - 1, _size + maxDst2road - 1 ), 
                                                !Tilemap::checkCorners );
  for (std::list<Tile*>::iterator itTiles = rect.begin(); itTiles != rect.end(); ++itTiles)
  {
    Tile* tile = *itTiles;

    if ( tile->get_terrain().isRoad() )
    {
      _accessRoads.push_back( tile );
    }
  }
}

unsigned char Construction::getMaxDistance2Road() const
{
  return 1;
  // it is default value
  // for houses - 2
}

void Construction::burn()
{
   deleteLater();
   Scenario::instance().getCity().disaster( getTile().getIJ(), DSTR_BURN );
}

void Construction::collapse()
{
   deleteLater();
   Scenario::instance().getCity().disaster( getTile().getIJ(), DSTR_COLLAPSE );
}

char Construction::getDesirabilityInfluence() const
{
  return 0;
}

void Construction::destroy()
{
  LandOverlay::destroy();
  _updateDesirabilityInfluence( false );
}

// I didn't decide what is the best approach: make Plaza as constructions or as upgrade to roads
Plaza::Plaza()
{
  // somewhere we need to delete original road and then we need to think
  // because as we remove original road we need to recompute adjacent tiles
  // or we will run into big troubles
 
  setType(B_PLAZA);
  setPicture(computePicture()); // 102 ~ 107
  _size = 1;
}


void Plaza::setTerrain(TerrainTile& terrain)
{
  //std::cout << "Plaza::setTerrain" << std::endl;
  bool isMeadow = terrain.isMeadow();  
  terrain.reset();
  terrain.setOverlay(this);
  terrain.setRoad(true);
  terrain.setMeadow(isMeadow);
  terrain.setOriginalImgId(terrain.getOriginalImgId() );
}

Picture& Plaza::computePicture()
{
  //std::cout << "Plaza::computePicture" << std::endl;
  return PicLoader::instance().get_picture( rcEntertaimentGroup, 102);
}

// Plazas can be built ONLY on top of existing roads
// Also in original game there was a bug:
// gamer could place any number of plazas on one road tile (!!!)

bool Plaza::canBuild(const TilePos& pos ) const
{
  //std::cout << "Plaza::canBuild" << std::endl;
  Tilemap& tilemap = Scenario::instance().getCity().getTilemap();

  bool is_constructible = true;

  std::list<Tile*> rect = tilemap.getFilledRectangle( pos, Size( _size ) ); // something very complex ???
  for (std::list<Tile*>::iterator itTiles = rect.begin(); itTiles != rect.end(); ++itTiles)
  {
    is_constructible &= (*itTiles)->get_terrain().isRoad();
  }

  return is_constructible;
}

Garden::Garden() : Construction(B_GARDEN, Size(1) )
{
  setPicture( Picture::load( rcEntertaimentGroup, 110) ); // 110 111 112 113
}

void Garden::setTerrain(TerrainTile &terrain)
{
  bool isMeadow = terrain.isMeadow();
  terrain.reset();
  terrain.setOverlay(this);
  terrain.setBuilding(true); // are gardens buildings or not???? try to investigate from original game
  terrain.setGarden(true);
  terrain.setMeadow(isMeadow);    
}

bool Garden::isWalkable() const
{
  return true;
}

Road::Road() : Construction( B_ROAD, Size(1) )
{
  setPicture( Picture::load( rcRoadGroup, 44));  // default picture for build tool
}

void Road::build(const TilePos& pos )
{
    Tilemap& tilemap = Scenario::instance().getCity().getTilemap();
    LandOverlay* saveOverlay = tilemap.at( pos ).get_terrain().getOverlay();

    Construction::build( pos );
    setPicture(computePicture());

    if( Aqueduct* aqua = safety_cast< Aqueduct* >( saveOverlay ) )
    {
        aqua->build( pos );
        return;
    }

  // update adjacent roads
  for (std::list<Tile*>::iterator itTile = _accessRoads.begin(); itTile != _accessRoads.end(); ++itTile)
  {
    Road* road = safety_cast< Road* >( (*itTile)->get_terrain().getOverlay() ); // let's think: may here different type screw up whole program?
    if( road )
    {
        road->computeAccessRoads();
        road->setPicture(road->computePicture());
    }
  }
  // NOTE: also we need to update accessRoads for adjacent building
  // how to detect them if MaxDistance2Road can be any
  // so let's recompute accessRoads for every _building_
  std::list<LandOverlay*> list = Scenario::instance().getCity().getOverlayList(); // it looks terrible!!!!
  for (std::list<LandOverlay*>::iterator itOverlay = list.begin(); itOverlay!=list.end(); ++itOverlay)
  {
    LandOverlay *overlay = *itOverlay;
    Building *construction = dynamic_cast<Building*>(overlay);
    if (construction != NULL) // if NULL then it ISN'T building
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
    TerrainTile& terrain = tilemap.at( pos ).get_terrain();

    if( safety_cast< Aqueduct* >( terrain.getOverlay() ) != 0 )
        return true;

    return false;
}


void Road::setTerrain(TerrainTile& terrain)
{
   terrain.reset();
   terrain.setOverlay( this );
   terrain.setRoad(true);
}

Picture& Road::computePicture()
{
   int i = getTile().getI();
   int j = getTile().getJ();

   std::list<Tile*> roads = getAccessRoads();
   int directionFlags = 0;  // bit field, N=1, E=2, S=4, W=8
   for (std::list<Tile*>::iterator itRoads = roads.begin(); itRoads!=roads.end(); ++itRoads)
   {
      Tile &tile = **itRoads;
      if (tile.getJ() > j)      { directionFlags += 1; } // road to the north
      else if (tile.getJ() < j) { directionFlags += 4; } // road to the south
      else if (tile.getI() > i) { directionFlags += 2; } // road to the east
      else if (tile.getI() < i) { directionFlags += 8; } // road to the west
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

   Picture *picture = &PicLoader::instance().get_picture( rcRoadGroup, index);
   return *picture;
}

bool Road::isWalkable() const
{
  return true;
}

Building::Building(const BuildingType type, const Size& size )
: Construction( type, size )
{
   _damageLevel = 0.0;
   _fireLevel = 0.0;
   _damageIncrement = 1;
   _fireIncrement = 1;
}

void Building::setTerrain(TerrainTile &terrain)
{
  // here goes the problem
  // when we reset tile, we delete information
  // about it's original information
  // try to fix
  bool isMeadow = terrain.isMeadow();
  terrain.reset();
  terrain.setOverlay(this);
  terrain.setBuilding(true);
  terrain.setMeadow(isMeadow);
}

void Building::timeStep(const unsigned long time)
{
   Construction::timeStep(time);

   if (time % 64 == 0)
   {
      _damageLevel += _damageIncrement;
      _fireLevel += _fireIncrement;
      if (_damageLevel >= 100)
      {
         std::cout << "Building destroyed!" << std::cout;
         collapse();
      }
      if (_fireLevel >= 100)
      {
         std::cout << "Building catch fire!" << std::cout;
         burn();
      }
   }
}

float Building::getDamageLevel()
{
   return _damageLevel;
}

void Building::setDamageLevel(const float value)
{
   _damageLevel = value;
}

float Building::getFireLevel()
{
   return _fireLevel;
}

void Building::setFireLevel(const float value)
{
   _fireLevel = value;
}


void Building::storeGoods(GoodStock &stock, const int amount)
{
   THROW("This building should not store any goods");
}

float Building::evaluateService(ServiceWalker &walker)
{
   float res = 0.0;
   ServiceType service = walker.getService();
   if (_reservedServices.count(service) == 1)
   {
      // service is already reserved
      return 0.0;
   }

   switch(service)
   {
   case S_ENGINEER:
      res = _damageLevel;
   break;

   case S_PREFECT:
      res = _fireLevel;
   break;
   }
   return res;
}

void Building::reserveService(const ServiceType service)
{
   // std::cout << "reserved service" << std::endl;
   _reservedServices.insert(service);
}

void Building::cancelService(const ServiceType service)
{
   // std::cout << "cancel service" << std::endl;
   _reservedServices.erase(service);
}

void Building::applyService(ServiceWalker &walker)
{
   // std::cout << "apply service" << std::endl;
   // remove service reservation
   ServiceType service = walker.getService();
   _reservedServices.erase(service);

   switch( service )
   {
   case S_ENGINEER:
     {
       _damageLevel = 0;
     }
   break;
   case S_PREFECT:
    {
      _fireLevel = 0;
    }
   break;
   }
}

float Building::evaluateTrainee(const WalkerTraineeType traineeType)
{
   float res = 0.0;

   if (_reservedTrainees.count(traineeType) == 1)
   {
      // don't allow two reservations of the same type
      return 0.0;
   }

   if (_traineeMap.count(traineeType) == 1)
   {
      int currentLevel = _traineeMap[traineeType];
      res = (float)( 101 - currentLevel );
   }

   return res;
}

void Building::reserveTrainee(const WalkerTraineeType traineeType)
{
   _reservedTrainees.insert(traineeType);
}

void Building::cancelTrainee(const WalkerTraineeType traineeType)
{
   _reservedTrainees.erase(traineeType);
}

void Building::applyTrainee(const WalkerTraineeType traineeType)
{
   _reservedTrainees.erase(traineeType);
   _traineeMap[traineeType] += 100;
}

void Building::serialize(OutputSerialStream &stream)
{
   Construction::serialize(stream);
   stream.write_int((int) _damageLevel, 1, 0, 100);  // approximation...
   stream.write_int((int) _fireLevel, 1, 0, 100);  // approximation...

   stream.write_int(_traineeMap.size(), 1, 0, WTT_MAX);
   for (std::map<WalkerTraineeType, int>::iterator itLevel = _traineeMap.begin(); itLevel != _traineeMap.end(); ++itLevel)
   {
      WalkerTraineeType traineeType = itLevel->first;
      int traineeLevel = itLevel->second;
      stream.write_int((int)traineeType, 1, 0, WTT_MAX);
      stream.write_int(traineeLevel, 1, 0, 200);
   }

   stream.write_int(_reservedTrainees.size(), 1, 0, WTT_MAX);
   for (std::set<WalkerTraineeType>::iterator itReservation = _reservedTrainees.begin(); itReservation != _reservedTrainees.end(); ++itReservation)
   {
      WalkerTraineeType traineeType = *itReservation;
      stream.write_int((int)traineeType, 1, 0, WTT_MAX);
   }
}

void Building::unserialize(InputSerialStream &stream)
{
   Construction::unserialize(stream);
   _damageLevel = (float)stream.read_int(1, 0, 100);
   _fireLevel = (float)stream.read_int(1, 0, 100);

   int size = stream.read_int(1, 0, WTT_MAX);
   for (int i=0; i<size; ++i)
   {
      WalkerTraineeType traineeType = (WalkerTraineeType) stream.read_int(1, 0, WTT_MAX);
      int traineeLevel = stream.read_int(1, 0, 200);
      _traineeMap[traineeType] = traineeLevel;
   }

   size = stream.read_int(1, 0, WTT_MAX);
   for (int i=0; i<size; ++i)
   {
      WalkerTraineeType traineeType = (WalkerTraineeType) stream.read_int(1, 0, WTT_MAX);
      _reservedTrainees.insert(traineeType);
   }
}

WorkingBuilding::WorkingBuilding(const BuildingType type, const Size& size)
: Building( type, size )
{
   _maxWorkers = 0;
   _currentWorkers = 0;
   _isActive = true;
}

void WorkingBuilding::setMaxWorkers(const int maxWorkers)
{
   _maxWorkers = maxWorkers;
}

int WorkingBuilding::getMaxWorkers() const
{
   return _maxWorkers;
}

void WorkingBuilding::setWorkers(const int currentWorkers)
{
   _currentWorkers = currentWorkers;
}

int WorkingBuilding::getWorkers() const
{
   return _currentWorkers;
}

void WorkingBuilding::setActive(const bool value)
{
   _isActive = value;
}

bool WorkingBuilding::isActive()
{
   return _isActive;
}

void WorkingBuilding::serialize(OutputSerialStream &stream)
{
   Building::serialize(stream);
   stream.write_int((int) _currentWorkers, 1, 0, 100);
}

void WorkingBuilding::unserialize(InputSerialStream &stream)
{
   Building::unserialize(stream);
   _currentWorkers = stream.read_int(1, 0, 100);
}

void WorkingBuilding::addWorkers( const int workers )
{
    _currentWorkers += workers;
}

Granary::Granary() : WorkingBuilding( B_GRANARY, Size(3) )
{
   setMaxWorkers(5);
   setWorkers(0);

   setPicture( Picture::load( ResourceGroup::commerce, 140));
   _fgPictures.resize(6);  // 1 upper level + 4 windows + animation
   int maxQty = 2400;
   _goodStore.setMaxQty(maxQty);
   _goodStore.setMaxQty(G_WHEAT, maxQty);
   _goodStore.setMaxQty(G_MEAT, maxQty);
   _goodStore.setMaxQty(G_FISH, maxQty);
   _goodStore.setMaxQty(G_FRUIT, maxQty);
   _goodStore.setMaxQty(G_VEGETABLE, maxQty);

   _goodStore.setCurrentQty(G_WHEAT, 300);

   _animation.load(ResourceGroup::commerce, 146, 7, Animation::straight);
   // do the animation in reverse
   _animation.load(ResourceGroup::commerce, 151, 6, Animation::reverse);
   PicLoader& ldr = PicLoader::instance();

   _fgPictures[0] = &ldr.get_picture( ResourceGroup::commerce, 141);
   _fgPictures[5] = _animation.getCurrentPicture();
   computePictures();
}

Granary* Granary::clone() const
{
   return new Granary(*this);
}


void Granary::timeStep(const unsigned long time)
{
   _animation.update( time );

   _fgPictures[5] = _animation.getCurrentPicture();
}

SimpleGoodStore& Granary::getGoodStore()
{
   return _goodStore;
}

void Granary::computePictures()
{
   int allQty = _goodStore.getCurrentQty();
   int maxQty = _goodStore.getMaxQty();

   for (int n = 0; n < 4; ++n)
   {
      // reset all window pictures
      _fgPictures[n+1] = NULL;
   }

   PicLoader& ldr = PicLoader::instance();
   if (allQty > 0)
   {
      _fgPictures[1] = &ldr.get_picture( ResourceGroup::commerce, 142);
   }
   if (allQty > maxQty * 0.25)
   {
      _fgPictures[2] = &ldr.get_picture( ResourceGroup::commerce, 143);
   }
   if (allQty > maxQty * 0.5)
   {
      _fgPictures[3] = &ldr.get_picture( ResourceGroup::commerce, 144);
   }
   if (allQty > maxQty * 0.9)
   {
      _fgPictures[4] = &ldr.get_picture( ResourceGroup::commerce, 145);
   }
}


GuiInfoBox* Granary::makeInfoBox( Widget* parent )
{
   GuiInfoGranary* box = new GuiInfoGranary( parent, *this);
   return box;
}

void Granary::serialize(OutputSerialStream &stream)
{
   WorkingBuilding::serialize(stream);
   _goodStore.serialize(stream);
}

void Granary::unserialize(InputSerialStream &stream)
{
   WorkingBuilding::unserialize(stream);
   _goodStore.unserialize(stream);
}

// housng1a 46 - governor's house    3 x 3
// housng1a 47 - governor's villa    4 x 4
// housng1a 48 - governor's palace   5 x 5

// govt     1  - small statue        1 x 1
// govt     2  - medium statue       2 x 2
// govt     3  - big statue          3 x 3

// land3a 43 44 - triumphal arch
// land3a 45 46 - triumphal arch

// transport 93 - missionaire post   2 x 2
// circus    1 ~ 18 hippodrome    5x(5 x 5)

MissionPost::MissionPost() : WorkingBuilding(B_MISSION_POST, Size(2) )
{
  setMaxWorkers(20);
  setWorkers(0);  
  setPicture(PicLoader::instance().get_picture("transport", 93));
}


SmallStatue::SmallStatue() : Building( B_STATUE1, Size(1) )
{
  setPicture( Picture::load(rcGovernmentGroup, 1));
}

MediumStatue::MediumStatue() : Building( B_STATUE2, Size(2) )
{
  setPicture( Picture::load(rcGovernmentGroup, 2));
}

BigStatue::BigStatue() : Building( B_STATUE3, Size(3))
{
  setPicture( Picture::load(rcGovernmentGroup, 3));
}

MissionPost*  MissionPost::clone()  const { return new MissionPost(*this);  }
SmallStatue*  SmallStatue::clone()  const { return new SmallStatue(*this);  }
MediumStatue* MediumStatue::clone() const { return new MediumStatue(*this); }
BigStatue*    BigStatue::clone()    const { return new BigStatue(*this);    }

GovernorsHouse::GovernorsHouse() : WorkingBuilding( B_GOVERNOR_HOUSE, Size(3) )
{
  setMaxWorkers(5);
  setWorkers(0);    
  setPicture(Picture::load(rcHousingGroup, 46));
}

GovernorsVilla::GovernorsVilla() : WorkingBuilding(B_GOVERNOR_VILLA, Size(4) )
{
  setMaxWorkers(10);
  setWorkers(0);    
  setPicture(Picture::load(rcHousingGroup, 47));
}

GovernorsPalace::GovernorsPalace() : WorkingBuilding(B_GOVERNOR_PALACE, Size( 5 ) )
{
  setMaxWorkers(15);
  setWorkers(0);  
  setPicture(Picture::load(rcHousingGroup, 48));
}

GovernorsHouse*  GovernorsHouse::clone()  const { return new GovernorsHouse(*this);  }
GovernorsVilla*  GovernorsVilla::clone()  const { return new GovernorsVilla(*this);  }
GovernorsPalace* GovernorsPalace::clone() const { return new GovernorsPalace(*this); }

Academy::Academy() : WorkingBuilding( B_MILITARY_ACADEMY, Size(3) )
{
  setMaxWorkers( 20 );
  setWorkers( 0 );
  setPicture(PicLoader::instance().get_picture( ResourceGroup::security, 18));
}

Barracks::Barracks() : WorkingBuilding( B_BARRACKS, Size( 3 ) )
{
  setMaxWorkers(5);
  setWorkers(0);  
  setPicture(PicLoader::instance().get_picture(ResourceGroup::security, 17));
}

Academy*  Academy::clone()  const { return new Academy(*this);  }
Barracks* Barracks::clone() const { return new Barracks(*this); }

NativeBuilding::NativeBuilding( const BuildingType type, const Size& size ) 
: Building( type, size )
{

}

void NativeBuilding::serialize(OutputSerialStream &stream) {Building::serialize(stream);}

void NativeBuilding::unserialize(InputSerialStream &stream) {Building::unserialize(stream);}

GuiInfoBox* NativeBuilding::makeInfoBox( Widget* parent )
{
  return new GuiBuilding( parent, *this);
}

NativeHut* NativeHut::clone() const
{
  return new NativeHut(*this);
}

NativeHut::NativeHut() : NativeBuilding( B_NATIVE_HUT, Size(1) )
{
  setPicture( Picture::load(rcHousingGroup, 49));
  //setPicture(PicLoader::instance().get_picture("housng1a", 50));
}

void NativeHut::serialize(OutputSerialStream &stream)  {Building::serialize(stream);}

void NativeHut::unserialize(InputSerialStream &stream) {Building::unserialize(stream);}

NativeCenter::NativeCenter() : NativeBuilding( B_NATIVE_CENTER, Size(2) )
{
  setPicture( Picture::load(rcHousingGroup, 51));
}

void NativeCenter::serialize(OutputSerialStream &stream)  {Building::serialize(stream);}

void NativeCenter::unserialize(InputSerialStream &stream) {Building::unserialize(stream);}

NativeCenter* NativeCenter::clone() const
{
  return new NativeCenter(*this);
}

NativeField::NativeField() : NativeBuilding( B_NATIVE_FIELD, Size(1) ) 
{
  setPicture(Picture::load(ResourceGroup::commerce, 13));  
}

void NativeField::serialize(OutputSerialStream &stream) {Building::serialize(stream);}

void NativeField::unserialize(InputSerialStream &stream) {Building::unserialize(stream);}

NativeField* NativeField::clone() const
{
  return new NativeField(*this);
}

Shipyard::Shipyard() : Building( B_SHIPYARD, Size(2) )
{
  setPicture( Picture::load("transport", 1));
  // also transport 2 3 4 check position of river on map
}

Shipyard* Shipyard::clone() const
{
   return new Shipyard(*this);
}

// dock pictures
// transport 5        animation = 6~16
// transport 17       animation = 18~28
// transport 29       animation = 30~40
// transport 41       animation = 42~51

Dock::Dock() : Building( B_DOCK, Size(2) )
{
  setPicture( Picture::load("transport", 5));  

  _animation.load( "transport", 6, 11);
  // now fill in reverse order
  _animation.load( "transport", 15, 10, Animation::reverse );
  
  _animation.setOffset( Point( 107, 61 ) );
  _fgPictures.resize(1);  
}

void Dock::timeStep(const unsigned long time)
{
  _animation.update( time );
  
  // takes current animation frame and put it into foreground
  _fgPictures.at(0) = _animation.getCurrentPicture(); 
}


Dock* Dock::clone() const
{
   return new Dock(*this);
}

// second arch pictures is land3a 45 + 46	

TriumphalArch::TriumphalArch() : Building( B_TRIUMPHAL_ARCH, Size(3) )
{
  setPicture( Picture::load( "land3a", 43 ) );
  getPicture().set_offset(0,116);
  _animation.load("land3a", 44, 1);
  _animation.setOffset( Point( 63, 97 ) );
  _fgPictures.resize(1);
  _fgPictures.at(0) = _animation.getCurrentPicture(); 
}

TriumphalArch* TriumphalArch::clone() const
{
   return new TriumphalArch(*this);
}



FortLegionnaire::FortLegionnaire() : Building( B_FORT_LEGIONNAIRE, Size(3) )
{
  setPicture( Picture::load(ResourceGroup::security, 12));

  Picture* logo = &Picture::load(ResourceGroup::security, 16);
  logo -> set_offset(80,10);
  _fgPictures.resize(1);
  _fgPictures.at(0) = logo;  
}

FortLegionnaire* FortLegionnaire::clone() const
{
   return new FortLegionnaire(*this);
}

FortMounted::FortMounted() : Building( B_FORT_MOUNTED, Size(3) )
{
  setPicture( Picture::load(ResourceGroup::security, 12));

  Picture* logo = &Picture::load(ResourceGroup::security, 15);
  logo -> set_offset(80,10);
  _fgPictures.resize(1);
  _fgPictures.at(0) = logo;
}

FortMounted* FortMounted::clone() const
{
   return new FortMounted(*this);
}

FortJaveline::FortJaveline() : Building( B_FORT_JAVELIN, Size(3) )
{
  setPicture( Picture::load(ResourceGroup::security, 12));

  Picture* logo = &Picture::load(ResourceGroup::security, 14);
  //std::cout << logo->get_xoffset() << " " << logo->get_yoffset() << " " << logo->get_width() << " " << logo->get_height() << std::endl;
  logo -> set_offset(80,10);
  _fgPictures.resize(1);
  _fgPictures.at(0) = logo;  
}

FortJaveline* FortJaveline::clone() const
{
   return new FortJaveline(*this);
}
