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

#ifndef __CAESARIA_COLOSSEUM_H_INCLUDED__
#define __CAESARIA_COLOSSEUM_H_INCLUDED__

#include "entertainment.hpp"

class Colosseum : public EntertainmentBuilding
{
public:
  Colosseum();
  virtual void deliverService();
  virtual Service::Type serviceType() const;
  virtual bool build(const CityAreaInfo &info);
  virtual std::string troubleDesc() const;

  bool isNeedGladiators() const;
  bool isShowGladiatorBattles() const;
  bool isShowLionBattles() const;

  DateTime lastAnimalBoutDate() const;
  DateTime lastGladiatorBoutDate() const;

  virtual void save(VariantMap &stream) const;
  virtual void load(const VariantMap &stream);

protected:
  virtual WalkerList _specificWorkers() const;

private:
  Service::Type _getServiceManType() const;

  class Impl;
  ScopedPtr<Impl> _d;
};

#endif //__CAESARIA_COLOSSEUM_H_INCLUDED__
