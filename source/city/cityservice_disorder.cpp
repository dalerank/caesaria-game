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
// Copyright 2012-2013 Dalerank, dalerankn8@gmail.com

#include "cityservice_disorder.hpp"
#include "city/helper.hpp"
#include "objects/constants.hpp"
#include "core/foreach.hpp"
#include "objects/house.hpp"
#include "walker/protestor.hpp"
#include "game/gamedate.hpp"

using namespace constants;

namespace constants
{
const int defaultCrimeLevel = 75;
}

namespace city
{

class Disorder::Impl
{
public:
  int minCrimeLevel;
};

SrvcPtr Disorder::create( PlayerCityPtr city )
{
  Disorder* ret = new Disorder( city );

  return SrvcPtr( ret );
}

std::string Disorder::getDefaultName(){  return "disorder";}

Disorder::Disorder(PlayerCityPtr city )
  : Srvc( *city.object(), Disorder::getDefaultName() ), _d( new Impl )
{
  _d->minCrimeLevel = defaultCrimeLevel;
}

void Disorder::update( const unsigned int time )
{
  if( !GameDate::isWeekChanged() )
    return;

  Helper helper( &_city );
  HouseList houses = helper.find<House>( building::house );

  WalkerList walkers = _city.getWalkers( walker::protestor );
  float cityCrimeKoeff = helper.getBalanceKoeff();

  HouseList criminalizedHouse;
  foreach( house, houses )
  {
    int crimeLvl = cityCrimeKoeff * (rand() % (int)( (*house)->getServiceValue( Service::crime )+1));
    if( crimeLvl >= _d->minCrimeLevel )
    {
      criminalizedHouse.push_back( *house );
    }
  }

  if( criminalizedHouse.size() > walkers.size() )
  {
    HouseList::iterator it = criminalizedHouse.begin();
    std::advance( it, rand() % criminalizedHouse.size() );
    (*it)->appendServiceValue( Service::crime, -defaultCrimeLevel / 2 );

    ProtestorPtr protestor = Protestor::create( &_city );
    protestor->send2City( *it );
  }
}

}//end namespace city
