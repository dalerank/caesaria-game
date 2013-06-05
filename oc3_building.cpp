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

#include "oc3_tile.hpp"
#include "oc3_scenario.hpp"
#include "oc3_servicewalker.hpp"
#include "oc3_exception.hpp"
#include "oc3_gui_info_box.hpp"
#include "oc3_building_data.hpp"
#include "oc3_factory_building.hpp"
#include "oc3_service_building.hpp"
#include "oc3_training_building.hpp"
#include "oc3_warehouse.hpp"
#include "oc3_gettext.hpp"
#include "oc3_time.hpp"
#include "oc3_burningruins.hpp"
#include "oc3_collapsedruins.hpp"
#include "oc3_water_buildings.hpp"
#include "oc3_constructionmanager.hpp"
#include "oc3_resourcegroup.hpp"
#include "oc3_variant.hpp"
#include "oc3_stringhelper.hpp"

#include <algorithm>

class LandOverlay::Impl
{
public:
  BuildingType buildingType;
};

LandOverlay::LandOverlay(const BuildingType type, const Size& size)
: _d( new Impl )
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
   return _d->buildingType;
}

void LandOverlay::setType(const BuildingType buildingType)
{
   _d->buildingType = buildingType;
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

  for (int dj = 0; dj < _size; ++dj)
  {
    for (int di = 0; di < _size; ++di)
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

std::string LandOverlay::getName()
{
  return _name;
}

void LandOverlay::save( VariantMap& stream ) const
{
  stream[ "pos" ] = getTile().getIJ();
  stream[ "buildingType" ] = (int)_d->buildingType;
  stream[ "picture" ] = Variant( _picture ? _picture->get_name() : std::string( "" ) );   
  stream[ "size" ] = _size;
  stream[ "isDeleted" ] = _isDeleted;
  stream[ "name" ] = Variant( _name );
}

void LandOverlay::load( const VariantMap& stream )
{
  _name = stream.get( "name" ).toString();
  _d->buildingType = (BuildingType)stream.get( "buildingType" ).toInt();
  _picture = &Picture::load( stream.get( "picture" ).toString() + ".png" );
  _size = stream.get( "size" ).toInt();
  _isDeleted = stream.get( "isDeleted" ).toBool();
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

  //return area for available tiles
  PtrTilesArea rect = tilemap.getFilledRectangle( pos, Size( _size ) );

  //on over map size
  if( rect.size() != _size * _size )
    return false;

  for( PtrTilesArea::iterator itTiles = rect.begin(); 
       itTiles != rect.end(); ++itTiles )
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

  PtrTilesList desirabilityArea = tilemap.getFilledRectangle( getTile().getIJ() - TilePos( 2, 2 ), 
    getTile().getIJ() + TilePos( 2 + getSize(), 2 + getSize() ) );

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

bool Construction::isNeedRoadAccess() const
{
  return true;
}

Garden::Garden() : Construction(B_GARDEN, Size(1) )
{
  // always set picture to 110 (tree garden) here, for sake of building preview
  // actual garden picture will be set upon building being constructed
  setPicture( Picture::load( ResourceGroup::entertaiment, 110 ) ); // 110 111 112 113
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

bool Garden::isNeedRoadAccess() const
{
  return false;
}

void Garden::build( const TilePos& pos )
{
  // this is the same arrangement of garden tiles as existed in C3
  int theGrid[2][2] = {{113, 110}, {112, 111}};

  Construction::build( pos );
  setPicture( Picture::load( ResourceGroup::entertaiment, theGrid[pos.getI() % 2][pos.getJ() % 2] ) );
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
        StringHelper::debug( 0xff, "Building destroyed!" );
        collapse();
      }
      if (_fireLevel >= 100)
      {
        StringHelper::debug( 0xff, "Building catch fire!" );
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

float Building::evaluateService(ServiceWalkerPtr walker)
{
   float res = 0.0;
   ServiceType service = walker->getService();
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

void Building::applyService( ServiceWalkerPtr walker)
{
   // std::cout << "apply service" << std::endl;
   // remove service reservation
   ServiceType service = walker->getService();
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

void Building::save( VariantMap& stream) const
{
    Construction::save( stream );
    stream[ "damageLevel" ] = _damageLevel;  
    stream[ "fireLevel" ] = _fireLevel;  

//    stream.write_int(_traineeMap.size(), 1, 0, WTT_MAX);
//    for (std::map<WalkerTraineeType, int>::iterator itLevel = _traineeMap.begin(); itLevel != _traineeMap.end(); ++itLevel)
//    {
//       WalkerTraineeType traineeType = itLevel->first;
//       int traineeLevel = itLevel->second;
//       stream.write_int((int)traineeType, 1, 0, WTT_MAX);
//       stream.write_int(traineeLevel, 1, 0, 200);
//    }
// 
//    stream.write_int(_reservedTrainees.size(), 1, 0, WTT_MAX);
//    for (std::set<WalkerTraineeType>::iterator itReservation = _reservedTrainees.begin(); itReservation != _reservedTrainees.end(); ++itReservation)
//    {
//       WalkerTraineeType traineeType = *itReservation;
//       stream.write_int((int)traineeType, 1, 0, WTT_MAX);
//    }
}

void Building::load( const VariantMap& stream )
{
//    Construction::unserialize(stream);
//    _damageLevel = (float)stream.read_int(1, 0, 100);
//    _fireLevel = (float)stream.read_int(1, 0, 100);
// 
//    int size = stream.read_int(1, 0, WTT_MAX);
//    for (int i=0; i<size; ++i)
//    {
//       WalkerTraineeType traineeType = (WalkerTraineeType) stream.read_int(1, 0, WTT_MAX);
//       int traineeLevel = stream.read_int(1, 0, 200);
//       _traineeMap[traineeType] = traineeLevel;
//    }
// 
//    size = stream.read_int(1, 0, WTT_MAX);
//    for (int i=0; i<size; ++i)
//    {
//       WalkerTraineeType traineeType = (WalkerTraineeType) stream.read_int(1, 0, WTT_MAX);
//       _reservedTrainees.insert(traineeType);
//    }
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

void WorkingBuilding::save( VariantMap& stream ) const
{
    Building::save( stream );
    stream[ "currentWorkers" ] = _currentWorkers;
}

void WorkingBuilding::load( const VariantMap& stream)
{
//    Building::unserialize(stream);
//    _currentWorkers = stream.read_int(1, 0, 100);
}

void WorkingBuilding::addWorkers( const int workers )
{
    _currentWorkers += workers;
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
  setPicture( Picture::load( ResourceGroup::govt, 1));
}

MediumStatue::MediumStatue() : Building( B_STATUE2, Size(2) )
{
  setPicture( Picture::load( ResourceGroup::govt, 2));
}

BigStatue::BigStatue() : Building( B_STATUE3, Size(3))
{
  setPicture( Picture::load( ResourceGroup::govt, 3));
}

GovernorsHouse::GovernorsHouse() : WorkingBuilding( B_GOVERNOR_HOUSE, Size(3) )
{
  setMaxWorkers(5);
  setWorkers(0);    
  setPicture(Picture::load( ResourceGroup::housing, 46));
}

GovernorsVilla::GovernorsVilla() : WorkingBuilding(B_GOVERNOR_VILLA, Size(4) )
{
  setMaxWorkers(10);
  setWorkers(0);    
  setPicture(Picture::load( ResourceGroup::housing, 47));
}

GovernorsPalace::GovernorsPalace() : WorkingBuilding(B_GOVERNOR_PALACE, Size( 5 ) )
{
  setMaxWorkers(15);
  setWorkers(0);  
  setPicture(Picture::load(ResourceGroup::housing, 48));
}

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

Shipyard::Shipyard() : Building( B_SHIPYARD, Size(2) )
{
  setPicture( Picture::load("transport", 1));
  // also transport 2 3 4 check position of river on map
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

FortLegionnaire::FortLegionnaire() : Building( B_FORT_LEGIONNAIRE, Size(3) )
{
  setPicture( Picture::load(ResourceGroup::security, 12));

  Picture* logo = &Picture::load(ResourceGroup::security, 16);
  logo -> set_offset(80,10);
  _fgPictures.resize(1);
  _fgPictures.at(0) = logo;  
}

FortMounted::FortMounted() : Building( B_FORT_MOUNTED, Size(3) )
{
  setPicture( Picture::load(ResourceGroup::security, 12));

  Picture* logo = &Picture::load(ResourceGroup::security, 15);
  logo -> set_offset(80,10);
  _fgPictures.resize(1);
  _fgPictures.at(0) = logo;
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
