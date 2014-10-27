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

#ifndef __CAESARIA_CITYSERVICE_PROSPERITY_H_INCLUDED__
#define __CAESARIA_CITYSERVICE_PROSPERITY_H_INCLUDED__

#include "cityservice.hpp"
#include "core/scopedptr.hpp"
#include "game/predefinitions.hpp"

namespace city
{

class ProsperityRating : public Srvc
{
public:
  typedef enum { cmHousesCap, cmHaveProfit, cmWorkless,
                 cmWorkersSalary, cmChange, cmPercentPlebs } Mark;
  static SrvcPtr create(PlayerCityPtr city);

  virtual void timeStep( const unsigned int time );
  int value() const;

  int getMark( Mark type ) const;

  static std::string defaultName();

  virtual VariantMap save() const;
  virtual void load(const VariantMap &stream);

private:
  ProsperityRating(PlayerCityPtr city);

  class Impl;
  ScopedPtr< Impl > _d;
};

typedef SmartPtr<ProsperityRating> ProsperityRatingPtr;

}//end namespace city

#endif //__CAESARIA_CITYSERVICE_PROSPERITY_H_INCLUDED__
