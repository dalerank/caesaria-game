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

#include "cityservice_disorder.hpp"
#include "objects/construction.hpp"
#include "city/helper.hpp"
#include "objects/constants.hpp"
#include "city/funds.hpp"
#include "core/foreach.hpp"
#include "objects/house.hpp"
#include "walker/rioter.hpp"
#include "game/gamedate.hpp"
#include "walker/mugger.hpp"
#include "events/showinfobox.hpp"

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

public:
  void generateMugger( PlayerCityPtr city, HousePtr house );
  void generateRioter( PlayerCityPtr city, HousePtr house );
  void generateProtestor( PlayerCityPtr city, HousePtr house );
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

  HouseList criminalizedHouse;
  _d->currentCrimeLevel = 0;
  _d->maxCrimeLevel = 0;

  foreach( house, houses )
  {
    int crimeLvl = (*house)->getServiceValue( Service::crime )+1;
    if( crimeLvl >= _d->minCrimeLevel )
    {
      criminalizedHouse.push_back( *house );
    }

    _d->currentCrimeLevel += crimeLvl;
    _d->maxCrimeLevel = std::max<int>( _d->maxCrimeLevel, crimeLvl );
  }

  if( houses.size() > 0 )
    _d->currentCrimeLevel /= houses.size();

  if( criminalizedHouse.size() > walkers.size() )
  {
    HouseList::iterator it = criminalizedHouse.begin();
    std::advance( it, math::random(criminalizedHouse.size()));

    int hCrimeLevel = (*it)->getServiceValue( Service::crime );

    int sentiment = _city.sentiment();
    int randomValue = math::random( 100 );
    if (sentiment >= 60)
    {
      if ( randomValue >= sentiment + 20 )
      {
        if ( hCrimeLevel > 50 )
        {
          _d->generateProtestor( &_city, *it );
        }
      }
    }
    else if ( sentiment >= 30 )
    {
      if ( randomValue >= sentiment + 40 )
      {
        if ( hCrimeLevel >= 70 )
        {
          _d->generateMugger( &_city, *it );
        }
        else if ( hCrimeLevel > 50 )
        {
          _d->generateProtestor( &_city, *it );
        }
      }
    }
    else // sentiment < 30
    {
      if ( randomValue >= sentiment + 50 )
      {
        if ( hCrimeLevel >= 90 )
        {
          _d->generateRioter( &_city, *it );
        }
        else if ( hCrimeLevel >= 70 )
        {
          _d->generateMugger( &_city, *it );
        }
        else if ( hCrimeLevel > 50 )
        {
          _d->generateProtestor( &_city, *it );
        }
      }
    }
  }
}

std::string Disorder::reasonDescr() const
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

void Disorder::Impl::generateMugger(PlayerCityPtr city, HousePtr house )
{
  house->appendServiceValue( Service::crime, -defaultCrimeLevel / 2 );

  int taxesThisYear = city->funds().getIssueValue( city::Funds::taxIncome );

  if( taxesThisYear > 20 )
  {
    int moneyStolen = taxesThisYear / 4;

    if( moneyStolen > 400 )
      moneyStolen = math::random( 400 );

    events::GameEventPtr e = events::ShowInfobox::create( "##money_stolen_title##", "##money_stolen_text##",
                                                          events::ShowInfobox::send2scribe, "/smk/mugging.smk" );
    e->dispatch();

    city->funds().resolveIssue( FundIssue( city::Funds::moneyStolen, -moneyStolen ) );
  }

  currentCrimeLevel++;
}

void Disorder::Impl::generateRioter(PlayerCityPtr city, HousePtr house)
{
  events::GameEventPtr e = events::ShowInfobox::create( "##rioter_in_city_title##", "##rioter_in_city_text##",
                                                        events::ShowInfobox::send2scribe, "/smk/spy_riot.smk" );

  RioterPtr protestor = Rioter::create( city );
  protestor->send2City( house );

  HouseList houses;
  houses << city->overlays();

  foreach( it, houses )
  {
    (*it)->appendServiceValue( Service::crime, -20 );
  }
}

void Disorder::Impl::generateProtestor(PlayerCityPtr city, HousePtr house)
{
  //ProtestorPtr protestor = Protestor::create( city );
  //protestor->send2City( house );
}

}//end namespace city
