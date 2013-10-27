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


#ifndef TRAINING_BUILDING_HPP
#define TRAINING_BUILDING_HPP

#include "working.hpp"

class TrainingBuilding : public WorkingBuilding
{
public:
   TrainingBuilding(const TileOverlayType type, const Size& size);

   virtual void timeStep(const unsigned long time);

   // called when a trainee is created
   virtual void deliverTrainee() = 0;

   virtual void save( VariantMap& stream) const;
   virtual void load( const VariantMap& stream);

private:
   int _trainingTimer;
   int _trainingDelay;
};

class ActorColony : public TrainingBuilding
{
public:
   ActorColony();
   virtual void deliverTrainee();
};

class GladiatorSchool : public TrainingBuilding
{
public:
   GladiatorSchool();
   virtual void deliverTrainee();
};

class LionsNursery : public TrainingBuilding
{
public:
   LionsNursery();
   virtual void deliverTrainee();
};

class WorkshopChariot : public TrainingBuilding
{
public:
   WorkshopChariot();
   virtual void deliverTrainee();
};

#endif
