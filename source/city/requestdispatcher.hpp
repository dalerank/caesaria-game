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

#ifndef _CAESARIA_CITYREQUESTDISPATCHER_H_INCLUDE_
#define _CAESARIA_CITYREQUESTDISPATCHER_H_INCLUDE_

#include "cityservice.hpp"
#include "request.hpp"
#include "core/signals.hpp"

namespace city
{

namespace request
{

class Dispatcher : public Srvc
{
public:
  static SrvcPtr create( PlayerCityPtr city );

  bool add(const VariantMap& stream , bool showMessage=true);
  virtual ~Dispatcher();

  static std::string defaultName();

  virtual void timeStep( const unsigned int time);
  virtual VariantMap save() const;
  virtual void load(const VariantMap &stream);

  bool haveCanceledRequest() const;

  RequestList requests() const;

private:
  Dispatcher(PlayerCityPtr city);

  class Impl;
  ScopedPtr<Impl> _d;
};

}//end namespace request

}//end namespace city

#endif //_CAESARIA_CITYREQUESTDISPATCHER_H_INCLUDE_
