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


#include "oc3_building_entertainment.hpp"
#include "oc3_positioni.hpp"
#include "oc3_resourcegroup.hpp"
#include "oc3_foreach.hpp"
#include "oc3_city.hpp"
#include "oc3_building_training.hpp"
#include "oc3_game_event_mgr.hpp"
#include "oc3_gettext.hpp"
#include "oc3_stringhelper.hpp"

EntertainmentBuilding::EntertainmentBuilding(const Service::Type service,
                                             const BuildingType type,
                                             const Size& size )
  : ServiceBuilding(service, type, size)
{
   switch( service )
   {
   case Service::theater:
      _traineeMap[WT_ACTOR] = 0;
   break;

   case Service::amphitheater:
      _traineeMap[WT_ACTOR] = 0;
      _traineeMap[WT_GLADIATOR] = 0;
   break;

   case Service::colloseum:
      _traineeMap[WT_GLADIATOR] = 0;
      _traineeMap[WT_TAMER] = 0;
   break;

   default:
     StringHelper::debug( 0xff, "Wrong entertainment service type %d", service );
   break;
   }
}

void EntertainmentBuilding::deliverService()
{
  // we need all trainees types for the show
  int minLevel = _getTraineeLevel();

  if( (getWorkers() <= 0) )
  {
    _getAnimation().stop();
    return;
  }

  int decreaseLevel = 10;
  // all trainees are there for the show!
  if( minLevel > 25 )
  {
    if( getWalkerList().empty() )
    {
      ServiceBuilding::deliverService();

      if( !getWalkerList().empty() )
      {
        _getAnimation().start();
        decreaseLevel = 25;
      }
    }
  }

  if( getWalkerList().empty() )
  {
    _getAnimation().stop(); //have no actors for the show
  }

  foreach( TraineeMap::value_type& item, _traineeMap )
  {
    item.second = math::clamp( item.second-decreaseLevel, 0, 100);
  }
}

int EntertainmentBuilding::getVisitorsNumber() const
{
  return 0;
}

unsigned int EntertainmentBuilding::getWalkerDistance() const
{
  return 35;
}

float EntertainmentBuilding::evaluateTrainee(const WalkerType traineeType)
{
  if( getWorkers() == 0 )
    return 0.0;

  return ServiceBuilding::evaluateTrainee( traineeType );
}

int EntertainmentBuilding::_getTraineeLevel()
{
  int minLevel = 100;
  foreach( TraineeMap::value_type item, _traineeMap )
  {
    minLevel = std::min( minLevel, item.second);
  }

  return minLevel;
}

Theater::Theater() : EntertainmentBuilding(Service::theater, B_THEATER, Size(2))
{
  //_getAnimation().load( ResourceGroup::entertaiment, 14, 21);
  //_getAnimation().setOffset( Point( 60, 36 ) );
  _getAnimation().stop();

  _fgPictures.resize(2);
}

void Theater::build(CityPtr city, const TilePos& pos)
{
  ServiceBuilding::build( city, pos );

  CityHelper helper( city );
  ActorColonyList actors = helper.getBuildings<ActorColony>( B_ACTOR_COLONY );

  if( actors.empty() )
  {
    GameEventMgr::append( WarningMessageEvent::create( _("##need_actor_colony##")) );
  }
}

void Theater::timeStep(const unsigned long time)
{
  EntertainmentBuilding::timeStep( time );
}

int Theater::getVisitorsNumber() const
{
  return 500;
}

void Theater::deliverService()
{
  EntertainmentBuilding::deliverService();

  _fgPictures[0] =  _getAnimation().isRunning()
                      ? Picture::load( ResourceGroup::entertaiment, 35 )
                      : Picture::getInvalid();
}

Amphitheater::Amphitheater() : EntertainmentBuilding(Service::amphitheater, B_AMPHITHEATER, Size(3))
{
  //setPicture( ResourceGroup::entertaiment, 1 );

  //_getAnimation().load( ResourceGroup::entertaiment, 2, 10);
  //_getAnimation().setOffset( Point( 100, 49 ) );
  _fgPictures.resize(2);
  //_fgPictures[0] = Picture::load( ResourceGroup::entertaiment, 12);
}

void Amphitheater::build(CityPtr city, const TilePos& pos)
{
  EntertainmentBuilding::build( city, pos );

  CityHelper helper( city );
  ActorColonyList actors = helper.getBuildings<ActorColony>( B_ACTOR_COLONY );

  if( actors.empty() )
  {
    GameEventMgr::append( WarningMessageEvent::create( _("##need_actor_colony##")) );
  }

  GladiatorSchoolList gladiators = helper.getBuildings<GladiatorSchool>( B_GLADIATOR_SCHOOL );
  if( actors.empty() )
  {
    GameEventMgr::append( WarningMessageEvent::create( _("##need_gladiator_school##")) );
  }
}

void Amphitheater::deliverService()
{
  EntertainmentBuilding::deliverService();

  _fgPictures[0] = _getAnimation().isRunning()
                         ? Picture::load( ResourceGroup::entertaiment, 12 )
                         : Picture::getInvalid();
}

Collosseum::Collosseum() : EntertainmentBuilding(Service::colloseum, B_COLLOSSEUM, Size(5) )
{
  //setPicture( Picture::load( ResourceGroup::entertaiment, 36));

  //_getAnimation().load( ResourceGroup::entertaiment, 37, 13);
  //_getAnimation().setOffset( Point( 122, 81 ) );
  _fgPictures.resize(2);
  _fgPictures[0] = Picture::load( ResourceGroup::entertaiment, 50);
}

//------------

Hippodrome::Hippodrome() : EntertainmentBuilding(Service::hippodrome, B_HIPPODROME, Size(5) )
{
  setPicture( Picture::load("circus", 5));
  Picture logo = Picture::load("circus", 3);
  Picture logo1 = Picture::load("circus", 1);
  logo.setOffset(150,181);
  logo1.setOffset(300,310);
  _fgPictures.resize(5);
  _fgPictures[ 0 ] = logo;
  _fgPictures[ 1 ] = logo1;
}

//-----------
