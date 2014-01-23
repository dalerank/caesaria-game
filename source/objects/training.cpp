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
// Copyright 2012-2013 Gregoire Athanase, gathanase@gmail.com
// Copyright 2012-2014 Dalerank, dalerankn8@gmail.com

#include "training.hpp"

#include "walker/trainee.hpp"
#include "core/exception.hpp"
#include "gui/info_box.hpp"
#include "core/gettext.hpp"
#include "core/variant.hpp"
#include "game/resourcegroup.hpp"
#include "city/city.hpp"
#include "constants.hpp"
#include "walker/lion_tamer.hpp"

using namespace constants;

TrainingBuilding::TrainingBuilding(const Type type, const Size& size )
  : WorkingBuilding( type, size )
{
   _trainingTimer = 0;
   _trainingDelay = 80;
}

void TrainingBuilding::timeStep(const unsigned long time)
{
   WorkingBuilding::timeStep(time);

   if( getWorkersCount() <= 0 )
   {
     return;
   }

   if( _trainingTimer == 0 )
   {
      deliverTrainee();
      _trainingTimer = _trainingDelay;
   }
   else if (_trainingTimer > 0)
   {
      _trainingTimer -= 1;
   }

   _animationRef().update( time );
   const Picture& pic = _animationRef().getFrame();
   if( pic.isValid() )
   {
      _fgPicturesRef().back() = _animationRef().getFrame();
   }
}

void TrainingBuilding::save( VariantMap& stream) const
{
  WorkingBuilding::save( stream );
  stream[ "trainingTimer" ] = _trainingTimer;
  stream[ "trainingDelay" ] = _trainingDelay;
}

void TrainingBuilding::load( const VariantMap& stream )
{
  WorkingBuilding::load( stream );
  _trainingTimer = (int)stream.get( "trainingTimer", 0 );
  _trainingDelay = (int)stream.get( "trainingDelay", 80 );
}

ActorColony::ActorColony() : TrainingBuilding( building::actorColony, Size(3) )
{
  _fgPicturesRef().resize(1);
}

void ActorColony::deliverTrainee()
{
  if( !getWalkers().empty() )
  {
    return;
  }

  TraineeWalkerPtr trainee = TraineeWalker::create( _getCity(), walker::actor );
  trainee->send2City( this );

  if( !trainee->isDeleted() )
  {
    addWalker( trainee.object() );
  }
}

void ActorColony::timeStep(const unsigned long time)
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

GladiatorSchool::GladiatorSchool() : TrainingBuilding( building::gladiatorSchool, Size(3))
{
  _fgPicturesRef().resize(1);
}

void GladiatorSchool::deliverTrainee()
{
   // std::cout << "Deliver trainee!" << std::endl;
  TraineeWalkerPtr trainee = TraineeWalker::create( _getCity(), walker::gladiator );
  trainee->send2City( this );
}

void GladiatorSchool::timeStep(const unsigned long time)
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

LionsNursery::LionsNursery() : TrainingBuilding( building::lionsNursery, Size(3) )
{
   _fgPicturesRef().resize(1);
}

void LionsNursery::timeStep(const unsigned long time)
{
  TrainingBuilding::timeStep( time );

  if( time % 15 == 1 )
  {
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
}

void LionsNursery::deliverTrainee()
{
  // std::cout << "Deliver trainee!" << std::endl;
  LionTamerPtr tamer = LionTamer::create( _getCity() );
  tamer->send2City( this, true );

  if( !tamer->isDeleted() )
  {
    addWalker( tamer.object() );
  }
}


WorkshopChariot::WorkshopChariot() : TrainingBuilding( building::chariotSchool, Size(3) )
{
  _fgPicturesRef().resize(1);
}

void WorkshopChariot::deliverTrainee()
{
   // std::cout << "Deliver trainee!" << std::endl;
  TraineeWalkerPtr trainee = TraineeWalker::create( _getCity(), walker::charioter );
  trainee->send2City( this );
}

void WorkshopChariot::timeStep(const unsigned long time)
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


