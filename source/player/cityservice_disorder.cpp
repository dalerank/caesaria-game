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

#include "cityservice_disorder.hpp"
#include "cityhelper.hpp"
#include "building/constants.hpp"
#include "core/foreach.hpp"
#include "building/house.hpp"
#include "walker/protestor.hpp"

using namespace constants;

namespace constants
{
const int defaultCrimeLevel = 75;
}

class CityServiceDisorder::Impl
{
public:
  PlayerCityPtr city;
  int minCrimeLevel;
};

CityServicePtr CityServiceDisorder::create( PlayerCityPtr city )
{
  CityServiceDisorder* ret = new CityServiceDisorder( city );

  return CityServicePtr( ret );
}

CityServiceDisorder::CityServiceDisorder(PlayerCityPtr city )
: CityService( "disorder" ), _d( new Impl )
{
  _d->city = city;
  _d->minCrimeLevel = defaultCrimeLevel;
}

void CityServiceDisorder::update( const unsigned int time )
{
  if( time % 22 != 1 )
    return;

  CityHelper helper( _d->city );
  HouseList houses = helper.find<House>( building::house );

  WalkerList walkers = _d->city->getWalkers( walker::protestor );
  float cityCrimeKoeff = helper.getBalanceKoeff();

  HouseList criminalizedHouse;
  foreach( HousePtr house, houses )
  {
    int crimeLvl = cityCrimeKoeff * (rand() % (house->getServiceValue( Service::crime )+1));
    if( crimeLvl >= _d->minCrimeLevel )
    {
      criminalizedHouse.push_back( house );
    }
  }

  if( criminalizedHouse.size() > walkers.size() )
  {
    HouseList::iterator it = criminalizedHouse.begin();
    std::advance( it, rand() % criminalizedHouse.size() );
    (*it)->appendServiceValue( Service::crime, -defaultCrimeLevel / 2 );

    ProtestorPtr protestor = Protestor::create( _d->city );
    protestor->send2City( *it );
  }
}
