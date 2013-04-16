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



#include "training_building.hpp"

#include <iostream>

#include "scenario.hpp"
#include "walker.hpp"
#include "exception.hpp"
#include "gui_info_box.hpp"
#include "gettext.hpp"

namespace {
static const char* rcEntertaimentGroup    = "entertainment";
}


TrainingBuilding::TrainingBuilding()
{
   setMaxWorkers(5);
   setWorkers(0);
   _trainingTimer = 0;
   _trainingDelay = 80;
}

void TrainingBuilding::timeStep(const unsigned long time)
{
   Building::timeStep(time);

   if (_trainingTimer == 0)
   {
      deliverTrainee();
      _trainingTimer = _trainingDelay;
   }
   else if (_trainingTimer > 0)
   {
      _trainingTimer -= 1;
   }

   _animation.update( time );
   Picture *pic = _animation.get_current_picture();
   if (pic != NULL)
   {
      int level = _fgPictures.size()-1;
      _fgPictures[level] = _animation.get_current_picture();
   }
}

// void TrainingBuilding::deliverTrainee()
// {
//    // make a service walker and send him to his wandering
//    ServiceWalker *walker = new ServiceWalker(_service);
//    walker->setServiceBuilding(*this);
//    walker->start();
// }

void TrainingBuilding::serialize(OutputSerialStream &stream)
{
   WorkingBuilding::serialize(stream);
   stream.write_int(_trainingTimer, 2, 0, 1000);
   stream.write_int(_trainingDelay, 2, 0, 1000);
}

void TrainingBuilding::unserialize(InputSerialStream &stream)
{
   WorkingBuilding::unserialize(stream);
   _trainingTimer = stream.read_int(2, 0, 1000);
   _trainingDelay = stream.read_int(2, 0, 1000);
}


//GuiInfoBox* TrainingBuilding::makeInfoBox()
//{
//   GuiInfoService* box = new GuiInfoService(*this);
//   return box;
//}


BuildingActor::BuildingActor()
{
   setType(B_ACTOR);
   _size = 3;
   setPicture(PicLoader::instance().get_picture(rcEntertaimentGroup, 81));

   AnimLoader animLoader(PicLoader::instance());
   animLoader.fill_animation(_animation, rcEntertaimentGroup, 82, 9);
   animLoader.change_offset(_animation, 68, -6);
   _fgPictures.resize(1);
}

BuildingActor* BuildingActor::clone() const
{
   return new BuildingActor(*this);
}

void BuildingActor::deliverTrainee()
{
   // std::cout << "Deliver trainee!" << std::endl;
   TraineeWalker *trainee = new TraineeWalker(WTT_ACTOR);
   trainee->setOriginBuilding(*this);
   trainee->start();
}

BuildingGladiator::BuildingGladiator()
{
   setType(B_GLADIATOR);
   _size = 3;
   setPicture(PicLoader::instance().get_picture(rcEntertaimentGroup, 51));

   AnimLoader animLoader(PicLoader::instance());
   animLoader.fill_animation(_animation, rcEntertaimentGroup, 52, 10);
   animLoader.change_offset(_animation, 62, 24);
   _fgPictures.resize(1);
}

BuildingGladiator* BuildingGladiator::clone() const
{
   return new BuildingGladiator(*this);
}

void BuildingGladiator::deliverTrainee()
{
   // std::cout << "Deliver trainee!" << std::endl;
   TraineeWalker *trainee = new TraineeWalker(WTT_GLADIATOR);
   trainee->setOriginBuilding(*this);
   trainee->start();
}


BuildingLion::BuildingLion()
{
   setType(B_LION);
   _size = 3;
   setPicture(PicLoader::instance().get_picture(rcEntertaimentGroup, 62));

   AnimLoader animLoader(PicLoader::instance());
   animLoader.fill_animation(_animation, rcEntertaimentGroup, 63, 18);
   animLoader.change_offset(_animation, 78, 21);
   _fgPictures.resize(1);
}

BuildingLion* BuildingLion::clone() const
{
   return new BuildingLion(*this);
}

void BuildingLion::deliverTrainee()
{
   // std::cout << "Deliver trainee!" << std::endl;
   TraineeWalker *trainee = new TraineeWalker(WTT_TAMER);
   trainee->setOriginBuilding(*this);
   trainee->start();
}


BuildingChariot::BuildingChariot()
{
   setType(B_CHARIOT);
   _size = 3;
   setPicture(PicLoader::instance().get_picture(rcEntertaimentGroup, 91));

   AnimLoader animLoader(PicLoader::instance());
   animLoader.fill_animation(_animation, rcEntertaimentGroup, 92, 10);
   animLoader.change_offset(_animation, 54, 23);
   _fgPictures.resize(1);
}

BuildingChariot* BuildingChariot::clone() const
{
   return new BuildingChariot(*this);
}

void BuildingChariot::deliverTrainee()
{
   // std::cout << "Deliver trainee!" << std::endl;
   TraineeWalker *trainee = new TraineeWalker(WTT_CHARIOT);
   trainee->setOriginBuilding(*this);
   trainee->start();
}


