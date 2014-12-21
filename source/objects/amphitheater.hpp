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

#ifndef __CAESARIA_AMPHITHEATER_H_INCLUDED__
#define __CAESARIA_AMPHITHEATER_H_INCLUDED__

#include "entertainment.hpp"

class Amphitheater : public EntertainmentBuilding
{
public:
  Amphitheater();

  virtual bool build( const CityAreaInfo& info );

  virtual void deliverService();
  virtual void timeStep(const unsigned long time);  
  virtual Service::Type serviceType() const;

  virtual std::string workersStateDesc() const;
  virtual void save(VariantMap& stream) const;
  virtual void load(const VariantMap& stream);

  DateTime lastShowDate() const;
  DateTime lastBoutsDate() const;

  bool isShowGladiatorBouts() const;
  bool isActorsShow() const;

  bool isNeed( constants::walker::Type type );

protected:
  virtual WalkerList _specificWorkers() const;

private:
  Service::Type _getServiceManType() const;

  class Impl;
  ScopedPtr< Impl > _d;
};

#endif //__CAESARIA_AMPHITHEATER_H_INCLUDED__
