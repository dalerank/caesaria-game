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

#include "romechastenerarmy.hpp"
#include "empire.hpp"
#include "city.hpp"

namespace world
{

class RomeChastenerArmy::Impl
{
public:
  int soldiersNumber;
};

ArmyPtr RomeChastenerArmy::create( EmpirePtr empire, CityPtr base )
{
  RomeChastenerArmy* ptr = new RomeChastenerArmy( empire );
  ptr->setBase( base );

  ArmyPtr ret( ptr );
  ret->drop();

  return ret;
}

void RomeChastenerArmy::setSoldiersNumber(unsigned int count) { _d->soldiersNumber = count; }

std::string RomeChastenerArmy::type() const { return CAESARIA_STR_EXT(RomeChastenerArmy); }
unsigned int RomeChastenerArmy::soldiersNumber() const { return _d->soldiersNumber; }

void RomeChastenerArmy::save(VariantMap& stream) const
{
  Army::save( stream );

  stream[ "soldiersNumber" ] = _d->soldiersNumber;
}

void RomeChastenerArmy::load(const VariantMap& stream)
{
  Army::load( stream );

  _d->soldiersNumber = stream.get( "soldiersNumber" );
}

RomeChastenerArmy::RomeChastenerArmy(EmpirePtr empire)
 : Army( empire ), _d( new Impl )
{

}

}
