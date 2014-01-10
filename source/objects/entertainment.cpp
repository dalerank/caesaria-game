// This file is part of CaesarIA.
//
// CaesarIA is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// CaesarIA is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with CaesarIA.  If not, see <http://www.gnu.org/licenses/>.


#include "entertainment.hpp"
#include "core/position.hpp"
#include "game/resourcegroup.hpp"
#include "core/foreach.hpp"
#include "city/helper.hpp"
#include "training.hpp"
#include "core/gettext.hpp"
#include "core/stringhelper.hpp"
#include "core/logger.hpp"
#include "objects/constants.hpp"

using namespace constants;

EntertainmentBuilding::EntertainmentBuilding(const Service::Type service,
                                             const TileOverlay::Type type,
                                             const Size& size )
  : ServiceBuilding(service, type, size)
{
   switch( service )
   {
   case Service::theater:
      _traineeMap[walker::actor] = 0;
   break;

   case Service::amphitheater:
      _traineeMap[walker::actor] = 0;
      _traineeMap[walker::gladiator] = 0;
   break;

   case Service::colloseum:
      _traineeMap[walker::gladiator] = 0;
      _traineeMap[walker::lionTamer] = 0;
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

  if( (getWorkersCount() <= 0) )
  {
    _animationRef().stop();
    return;
  }

  int decreaseLevel = 10;
  // all trainees are there for the show!
  if( minLevel > 25 )
  {
    if( getWalkers().empty() )
    {
      ServiceBuilding::deliverService();

      if( !getWalkers().empty() )
      {
        _animationRef().start();
        decreaseLevel = 25;
      }
    }
  }

  if( getWalkers().empty() )
  {
    _animationRef().stop(); //have no actors for the show
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

float EntertainmentBuilding::evaluateTrainee(walker::Type traineeType)
{
  if( getWorkersCount() == 0 )
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
  _fgPicturesRef().resize(2);
}

void Theater::build(PlayerCityPtr city, const TilePos& pos)
{
  ServiceBuilding::build( city, pos );

  CityHelper helper( city );
  ActorColonyList actors = helper.find<ActorColony>( building::actorColony );

  if( actors.empty() )
  {
    _setError( _("##need_actor_colony##") );
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

  if( _animationRef().isRunning() )
  {
    _fgPicturesRef().front() = Picture::load( ResourceGroup::entertaiment, 35 );
  }
  else
  {
    _fgPicturesRef().front() = Picture::getInvalid();
    _fgPicturesRef().back() = Picture::getInvalid();
  }
}

Collosseum::Collosseum() : EntertainmentBuilding(Service::colloseum, building::colloseum, Size(5) )
{
  setPicture( Picture::load( ResourceGroup::entertaiment, 36));

  _animationRef().load( ResourceGroup::entertaiment, 37, 13);
  _animationRef().setOffset( Point( 122, 81 ) );

  _fgPicturesRef().resize(2);
}

void Collosseum::deliverService()
{
  EntertainmentBuilding::deliverService();

  if( _animationRef().isRunning() )
  {
    _fgPicturesRef().front() = Picture::load( ResourceGroup::entertaiment, 50 );
  }
  else
  {
    _fgPicturesRef().front() = Picture::getInvalid();
    _fgPicturesRef().back() = Picture::getInvalid();
  }
}

void Collosseum::build(PlayerCityPtr city, const TilePos& pos)
{
  ServiceBuilding::build( city, pos );

  CityHelper helper( city );
  GladiatorSchoolList glSchools = helper.find<GladiatorSchool>( building::gladiatorSchool );
  LionsNurseryList lionsNs = helper.find<LionsNursery>( building::lionsNursery );

  if( glSchools.empty() )
  {
    _setError( _("##need_gladiator_school##") );
  }

  if( lionsNs.empty() )
  {
    _setError( _("##need_gladiator_school##") );
  }
}

bool Collosseum::isNeedGladiators() const
{
  CityHelper helper( _getCity() );
  GladiatorSchoolList colloseums = helper.find<GladiatorSchool>( building::gladiatorSchool );

  return colloseums.empty();
}

//------------

Hippodrome::Hippodrome() : EntertainmentBuilding(Service::hippodrome, building::hippodrome, Size(5) )
{
  setPicture( "circus", 5 );
  Picture logo = Picture::load("circus", 3);
  Picture logo1 = Picture::load("circus", 1);
  logo.setOffset(150,181);
  logo1.setOffset(300,310);
  _fgPicturesRef().resize(5);
  _fgPicturesRef().at(0) = logo;
  _fgPicturesRef().at(1) = logo1;
}

//-----------
