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

#include "oc3_scenario.hpp"
#include "oc3_exception.hpp"
#include "oc3_variant.hpp"
#include "oc3_build_options.hpp"
#include "oc3_win_targets.hpp"

class Scenario::Impl
{
public:
  City city;
  std::string description;
  CityWinTargets targets;
};

Scenario& Scenario::instance()
{
  static Scenario inst;
  return inst;
}

Scenario::Scenario() : _d( new Impl )
{
  _d->description = "";
}

City& Scenario::getCity()
{
  return _d->city;
}

const City& Scenario::getCity() const
{
  return _d->city;
}

std::string Scenario::getDescription() const
{
  return _d->description;
}

void Scenario::save( VariantMap& stream ) const
{
  stream[ "description" ] = Variant( getDescription() );
}

void Scenario::load( const VariantMap& stream)
{
}

CityWinTargets& Scenario::getWinTargets()
{
  return _d->targets;
}

Scenario::~Scenario()
{

}