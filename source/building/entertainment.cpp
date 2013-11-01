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


#include "entertainment.hpp"
#include "core/position.hpp"
#include "game/resourcegroup.hpp"
#include "core/foreach.hpp"
#include "game/city.hpp"
#include "training.hpp"
#include "core/gettext.hpp"
#include "core/stringhelper.hpp"
#include "events/event.hpp"
#include "core/logger.hpp"
#include "building/constants.hpp"

using namespace constants;

EntertainmentBuilding::EntertainmentBuilding(const Service::Type service,
                                             const TileOverlay::Type type,
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
     Logger::warning( "Wrong entertainment service type %d", service );
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

bool EntertainmentBuilding::isShow() const
{
  return true;
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

Theater::Theater() : EntertainmentBuilding(Service::theater, building::theater, Size(2))
{
  //_getAnimation().load( ResourceGroup::entertaiment, 14, 21);
  //_getAnimation().setOffset( Point( 60, 36 ) );
  _getAnimation().stop();

  _getForegroundPictures().resize(2);
}

void Theater::build(CityPtr city, const TilePos& pos)
{
  ServiceBuilding::build( city, pos );

  CityHelper helper( city );
  ActorColonyList actors = helper.find<ActorColony>( building::B_ACTOR_COLONY );

  if( actors.empty() )
  {
    events::GameEventPtr event = events::WarningMessageEvent::create( _("##need_actor_colony##"));
    event->dispatch();
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

  _getForegroundPictures().at(0) =  _getAnimation().isRunning()
                      ? Picture::load( ResourceGroup::entertaiment, 35 )
                      : Picture::getInvalid();
}

Amphitheater::Amphitheater() : EntertainmentBuilding(Service::amphitheater, building::amphitheater, Size(3))
{
  //setPicture( ResourceGroup::entertaiment, 1 );
  //_getAnimation().load( ResourceGroup::entertaiment, 2, 10);
  //_getAnimation().setOffset( Point( 100, 49 ) );
  _getForegroundPictures().resize(2);
  //_fgPictures[0] = Picture::load( ResourceGroup::entertaiment, 12);
}

void Amphitheater::build(CityPtr city, const TilePos& pos)
{
  EntertainmentBuilding::build( city, pos );

  CityHelper helper( city );
  ActorColonyList actors = helper.find<ActorColony>( building::B_ACTOR_COLONY );

  if( actors.empty() )
  {
    events::GameEventPtr event = events::WarningMessageEvent::create( _("##need_actor_colony##"));
    event->dispatch();
  }

  GladiatorSchoolList gladiators = helper.find<GladiatorSchool>( building::B_GLADIATOR_SCHOOL );
  if( actors.empty() )
  {
    events::GameEventPtr event = events::WarningMessageEvent::create( _("##need_gladiator_school##"));
    event->dispatch();
  }
}

void Amphitheater::deliverService()
{
  EntertainmentBuilding::deliverService();

  _getForegroundPictures().at(0) = _getAnimation().isRunning()
                         ? Picture::load( ResourceGroup::entertaiment, 12 )
                         : Picture::getInvalid();
}

Collosseum::Collosseum() : EntertainmentBuilding(Service::colloseum, building::B_COLLOSSEUM, Size(5) )
{
  //setPicture( Picture::load( ResourceGroup::entertaiment, 36));

  //_getAnimation().load( ResourceGroup::entertaiment, 37, 13);
  //_getAnimation().setOffset( Point( 122, 81 ) );
  _getForegroundPictures().resize(2);
  _getForegroundPictures().at(0) = Picture::load( ResourceGroup::entertaiment, 50);
}

//------------

Hippodrome::Hippodrome() : EntertainmentBuilding(Service::hippodrome, building::B_HIPPODROME, Size(5) )
{
  setPicture( Picture::load("circus", 5));
  Picture logo = Picture::load("circus", 3);
  Picture logo1 = Picture::load("circus", 1);
  logo.setOffset(150,181);
  logo1.setOffset(300,310);
  _getForegroundPictures().resize(5);
  _getForegroundPictures().at(0) = logo;
  _getForegroundPictures().at(1) = logo1;
}

//-----------
