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
//
// Copyright 2012-2014 Dalerank, dalerankn8@gmail.com

#include "entertainment.hpp"
#include "core/position.hpp"
#include "game/resourcegroup.hpp"
#include "core/foreach.hpp"
#include "city/helper.hpp"
#include "training.hpp"
#include "core/stringhelper.hpp"
#include "core/logger.hpp"
#include "objects/constants.hpp"

using namespace constants;

class EntertainmentBuilding::Impl
{
public:
  EntertainmentBuilding::NecessaryWalkers necWalkers;
  unsigned int showCounter;
};

EntertainmentBuilding::EntertainmentBuilding(const Service::Type service,
                                             const TileOverlay::Type type,
                                             Size size )
  : ServiceBuilding(service, type, size), _d( new Impl )
{
  _d->showCounter = 0;
}

EntertainmentBuilding::~EntertainmentBuilding()
{

}

void EntertainmentBuilding::deliverService()
{
  // we need all trainees types for the show
  if( numberWorkers() <= 0 )
  {
    _animationRef().stop();
    return;
  }

  bool isWalkerReady = _isWalkerReady();

  int decreaseLevel = 10;
  // all trainees are there for the show!
  if( isWalkerReady )
  {
    if( getWalkers().empty() )
    {
      ServiceBuilding::deliverService();
      _d->showCounter++;

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

  foreach( item, _d->necWalkers )
  {
    int level = getTraineeValue( *item );
    setTraineeValue( *item, math::clamp( level - decreaseLevel, 0, 100) );
  }
}

int EntertainmentBuilding::getVisitorsNumber() const{  return 0;}
unsigned int EntertainmentBuilding::getWalkerDistance() const {  return 35; }

float EntertainmentBuilding::evaluateTrainee(walker::Type traineeType)
{
  if( numberWorkers() == 0 )
    return 0.0;

  return ServiceBuilding::evaluateTrainee( traineeType );
}

bool EntertainmentBuilding::isShow() const { return getAnimation().isRunning(); }
unsigned int EntertainmentBuilding::getShowsCount() const { return _d->showCounter; }

void EntertainmentBuilding::save(VariantMap& stream) const
{
  ServiceBuilding::save( stream );
  stream[ "showCounter" ] = _d->showCounter;
}

void EntertainmentBuilding::load(const VariantMap& stream)
{
  ServiceBuilding::load( stream );
  _d->showCounter = (int)stream.get( "showCounter" );
}

EntertainmentBuilding::NecessaryWalkers EntertainmentBuilding::getNecessaryWalkers() const { return _d->necWalkers; }
void EntertainmentBuilding::_addNecessaryWalker(walker::Type type){  _d->necWalkers.push_back( type );}

bool EntertainmentBuilding::_isWalkerReady()
{
  int maxLevel = 0;
  foreach( item, _d->necWalkers )
  {  maxLevel = std::max( maxLevel, getTraineeValue( *item ) ); }

  return maxLevel;
}

Theater::Theater() : EntertainmentBuilding(Service::theater, building::theater, Size(2))
{
  _fgPicturesRef().resize(2);
  _addNecessaryWalker( walker::actor );
}

void Theater::build(PlayerCityPtr city, const TilePos& pos)
{
  ServiceBuilding::build( city, pos );

  city::Helper helper( city );
  ActorColonyList actors = helper.find<ActorColony>( building::actorColony );

  if( actors.empty() )
  {
    _setError( "##need_actor_colony##" );
  }
}

void Theater::timeStep(const unsigned long time) {  EntertainmentBuilding::timeStep( time );}
int Theater::getVisitorsNumber() const {  return 500; }

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

Hippodrome::Hippodrome() : EntertainmentBuilding(Service::hippodrome, building::hippodrome, Size(5) )
{
  setPicture( "circus", 5 );
  Picture logo = Picture::load("circus", 3);
  Picture logo1 = Picture::load("circus", 1);
  logo.setOffset( Point( 150,181 ) );
  logo1.setOffset( Point( 300,310 ) );
  _fgPicturesRef().resize(5);
  _fgPicturesRef()[0] = logo;
  _fgPicturesRef()[1] = logo1;

  _addNecessaryWalker( walker::charioter );
}

std::string Hippodrome::getTrouble() const
{
  std::string ret = EntertainmentBuilding::getTrouble();

  if( ret.empty() )
  {
    ret = isRacesCarry() ? "##trouble_hippodrome_full_work##" : "##trouble_hippodrome_no_charioters##";
  }

  return ret;
}

bool Hippodrome::isRacesCarry() const{ return getTraineeValue( walker::charioter ) > 0; }
