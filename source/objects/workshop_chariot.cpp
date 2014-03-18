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

#include "workshop_chariot.hpp"

#include "walker/trainee.hpp"
#include "core/exception.hpp"
#include "gui/info_box.hpp"
#include "core/gettext.hpp"
#include "core/variant.hpp"
#include "game/resourcegroup.hpp"
#include "city/city.hpp"
#include "constants.hpp"

using namespace constants;

WorkshopChariot::WorkshopChariot() : TrainingBuilding( building::chariotSchool, Size(3) )
{
  _fgPicturesRef().resize(1);
}

void WorkshopChariot::deliverTrainee()
{
   // std::cout << "Deliver trainee!" << std::endl;
  TraineeWalkerPtr trainee = TraineeWalker::create( _city(), walker::charioter );
  trainee->send2City( this );
}

void WorkshopChariot::timeStep(const unsigned long time)
{
  TrainingBuilding::timeStep( time );

  if( numberWorkers() > 0 )
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


