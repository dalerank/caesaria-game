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


#ifndef _CAESARIA_TRAINING_H_INCLUDE_
#define _CAESARIA_TRAINING_H_INCLUDE_

#include "working.hpp"

class TrainingBuilding : public WorkingBuilding
{
public:
   TrainingBuilding(const object::Type type, const Size& size);

   virtual void timeStep(const unsigned long time);

   // called when a trainee is created
   virtual void deliverTrainee() = 0;

   virtual void save( VariantMap& stream) const;
   virtual void load( const VariantMap& stream);

protected:

private:
   DateTime _lastSendDate;
   int _trainingDelay;
};

class GladiatorSchool : public TrainingBuilding
{
public:
   GladiatorSchool();
   virtual void deliverTrainee();
   virtual void timeStep(const unsigned long time);
};

class LionsNursery : public TrainingBuilding
{
public:
   LionsNursery();
   virtual void deliverTrainee();
   virtual void timeStep(const unsigned long time);
};

#endif //_CAESARIA_TRAINING_H_INCLUDE_
