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

#include "barracks.hpp"
#include "constants.hpp"
#include "game/resourcegroup.hpp"
#include "walker/trainee.hpp"
#include "game/city.hpp"

using namespace constants;

Barracks::Barracks() : TrainingBuilding( building::barracks, Size( 3 ) )
{
  setMaxWorkers(5);
  setPicture( ResourceGroup::security, 17 );
}

void Barracks::deliverTrainee()
{
  TraineeWalkerPtr trainee = TraineeWalker::create( _getCity(), walker::soldier );
  trainee->setOriginBuilding(*this);

  CityHelper helper( _getCity() );
  FortList forts = helper.find<Fort>( building::militaryGroup );

  for( FortList::iterator it=forts.begin(); it != forts.end(); it++ )
  {
    FortPtr fort = *it;
    if( fi)
  }
}

void Barracks::timeStep(const unsigned long time)
{
  TrainingBuilding::timeStep( time );

  if( getWorkersCount() > 0 )
  {
    if( _animationRef().isStopped() )
    {
      _animationRef().start();
    }
  }
  else if( _animationRef().isRunning() )
  {
    _animationRef().stop();
  }
}
