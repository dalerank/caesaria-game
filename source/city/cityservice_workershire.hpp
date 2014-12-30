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

#ifndef __CAESARIA_CITYSERVICE_WORKERSHIRE_H_INCLUDED__
#define __CAESARIA_CITYSERVICE_WORKERSHIRE_H_INCLUDED__

#include "cityservice.hpp"
#include "core/scopedptr.hpp"
#include "gfx/tileoverlay.hpp"
#include "core/predefinitions.hpp"
#include "core/priorities.hpp"
#include "industry.hpp"

namespace city
{

class WorkersHire : public Srvc
{
public:
  static SrvcPtr create( PlayerCityPtr city );
  static std::string defaultName();
  virtual void timeStep( const unsigned int time );
  void setRecruterDistance( const unsigned int distance );
  void setIndustryPriority( industry::Type industry, int priority );
  int getPriority( industry::Type industry );
  const HirePriorities& priorities() const;

  virtual VariantMap save() const;
  virtual void load(const VariantMap& stream);
private:
  WorkersHire( PlayerCityPtr city );
 
  class Impl;
  ScopedPtr< Impl > _d;
};

typedef SmartPtr<WorkersHire> WorkersHirePtr;

}//end namespace city
#endif //__CAESARIA_CITYSERVICE_WORKERSHIRE_H_INCLUDED__
