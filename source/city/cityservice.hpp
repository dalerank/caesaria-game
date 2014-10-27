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

#ifndef __CAESARIA_CITYSERVICE_H_INCLUDED__
#define __CAESARIA_CITYSERVICE_H_INCLUDED__

#include "core/smartptr.hpp"
#include "core/variant.hpp"
#include "predefinitions.hpp"
#include "core/scopedptr.hpp"

namespace city
{

class Srvc : public ReferenceCounted
{
public:
  virtual void timeStep( const unsigned int time ) = 0;

  std::string name() const;
  void setName( const std::string& name  );

  virtual bool isDeleted() const;
  
  virtual void destroy();

  virtual VariantMap save() const;
  virtual void load(const VariantMap& stream);

  virtual ~Srvc();

protected:
  Srvc( PlayerCityPtr city, const std::string& name );
  PlayerCityPtr _city() const;

private:
  class Impl;
  ScopedPtr<Impl> _d;
};

typedef SmartPtr<Srvc> SrvcPtr;

}//end namespace city

#endif//__CAESARIA_CITYSERVICE_H_INCLUDED__
