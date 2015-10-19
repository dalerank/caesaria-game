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

#include "theater.hpp"
#include "constants.hpp"
#include "city/statistic.hpp"
#include "game/resourcegroup.hpp"
#include "actor_colony.hpp"
#include "walker/walker.hpp"
#include "objects_factory.hpp"

using namespace gfx;

REGISTER_CLASS_IN_OVERLAYFACTORY(object::theater, Theater)

Theater::Theater() : EntertainmentBuilding(Service::theater, object::theater, Size(2))
{
  _fgPictures().resize(2);

  _addNecessaryWalker( walker::actor );
}

bool Theater::build( const city::AreaInfo& info )
{
  ServiceBuilding::build( info );

  int actors_n = info.city->statistic().objects.count<ActorColony>();

  if( !actors_n )
    _setError( "##need_actor_colony##" );

  return true;
}

void Theater::timeStep(const unsigned long time) { EntertainmentBuilding::timeStep( time );}
int Theater::currentVisitors() const { return 500; }

void Theater::deliverService()
{
  EntertainmentBuilding::deliverService();

  if( _animation().isRunning() )
  {
    _fgPictures().front().load( ResourceGroup::entertainment, 35 );
  }
  else
  {
    _fgPictures().front() = Picture::getInvalid();
    _fgPictures().back() = Picture::getInvalid();
  }
}

bool Theater::mayWork() const {  return (numberWorkers() > 0 && traineeValue(walker::actor) > 0); }
int Theater::maxVisitors() const { return 500; }

WalkerList Theater::_specificWorkers() const
{
  WalkerList ret;

  for( auto wlk : walkers() )
  {
    if( wlk->type() == walker::actor )
      ret << wlk;
  }

  return ret;
}
