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
#include "objects/construction.hpp"
#include "city/helper.hpp"
#include "objects/constants.hpp"
#include "core/foreach.hpp"
#include "objects/house.hpp"
#include "walker/protestor.hpp"
#include "game/gamedate.hpp"

using namespace constants;

namespace
{
const int defaultCrimeLevel = 75;
const int crimeDescLimiter = 10;
}

namespace city
{

class Disorder::Impl
{
public:
  int minCrimeLevel;
  int currentCrimeLevel;
  int maxCrimeLevel;
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
  _d->currentCrimeLevel = 0;
  _d->maxCrimeLevel = 0;
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
  _d->currentCrimeLevel = 0;
  _d->maxCrimeLevel = 0;
  foreach( house, houses )
  {
    int crimeLvl = cityCrimeKoeff * (rand() % (int)( (*house)->getServiceValue( Service::crime )+1));
    if( crimeLvl >= _d->minCrimeLevel )
    {
      criminalizedHouse.push_back( *house );
    }

    _d->currentCrimeLevel = ( _d->currentCrimeLevel + crimeLvl ) / 2;
    _d->maxCrimeLevel = std::max<int>( _d->maxCrimeLevel, crimeLvl );
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

std::string Disorder::getReason() const
{
  int crimeLevel = math::clamp<int>( _d->currentCrimeLevel / crimeDescLimiter, 0, crimeDescLimiter-1 );
  std::string crimeDesc[ crimeDescLimiter ] = { "##advchief_no_crime##", "##advchief_very_low_crime##", "##advchief_low_crime##",
                                                "##advchief_some_crime##", "##advchief_which_crime##", "##advchief_more_crime##",
                                                "##advchief_simple_crime##", "##advchief_average_crime##", "##advchief_high_crime##",
                                                "##advchief_veryhigh_crime##"};

  StringArray troubles;
  troubles << crimeDesc[ crimeLevel ];

  if( _d->maxCrimeLevel > defaultCrimeLevel )
  {
    troubles << "##advchief_high_crime_in_district##";
  }
  else if( _d->maxCrimeLevel > defaultCrimeLevel / 2 )
  {
    troubles << "##advchief_which_crime_in_district##";
  }
  else if( _d->maxCrimeLevel > defaultCrimeLevel / 5 )
  {
    troubles << "##advchief_low_crime##";
  }

  return troubles.rand();
}

unsigned int Disorder::value() const
{
  return _d->currentCrimeLevel;
}

}//end namespace city
