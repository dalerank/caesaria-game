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
#include "city/helper.hpp"
#include "game/resourcegroup.hpp"
#include "actor_colony.hpp"

using namespace constants;

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
int Theater::visitorsNumber() const {  return 500; }

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
