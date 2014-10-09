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

#include "cityservice.hpp"
#include "core/logger.hpp"

namespace city
{

class Srvc::Impl
{
public:
  std::string name;
};

std::string Srvc::name() const { return _d->name; }

void Srvc::setName(const std::string& name) { _d->name = name; }

bool Srvc::isDeleted() const { return false; }

void Srvc::destroy( PlayerCityPtr ) {}

VariantMap Srvc::save() const { return VariantMap(); }

void Srvc::load(const VariantMap& stream) {}

Srvc::~Srvc()
{
  Logger::warning( "CityServices: remove " + name() );
}

Srvc::Srvc(const std::string& name)
  : _d( new Impl )
{
  _d->name = name;
  Logger::warning( "CityServices: create " + name );
}



}
