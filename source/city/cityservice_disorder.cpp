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
#include "city/statistic.hpp"
#include "objects/constants.hpp"
#include "city/funds.hpp"
#include "core/foreach.hpp"
#include "objects/house.hpp"
#include "walker/rioter.hpp"
#include "core/variant_map.hpp"
#include "game/gamedate.hpp"
#include "events/event.hpp"
#include "walker/mugger.hpp"
#include "events/showinfobox.hpp"
#include "cityservice_factory.hpp"
#include "config.hpp"

using namespace constants;
using namespace config;

namespace
{
const int minCityTax4mugger = 20;

const int crimedecOnRioterBirth = 20;

const int minSentiment4protest = 60;
const int minSentiment4mugger = 30;

const int defaultCrimeLevel = 75;
const int crimeDescLimiter = 10;
}

namespace city
{

REGISTER_SERVICE_IN_FACTORY(Disorder,disorder)

class Disorder::Impl
{
public:
  int minCrimeLevel;
  int currentCrimeLevel;
  int maxCrimeLevel;
  int rioterInThisYear;
  int rioterInLastYear;
  DateTime lastMessageDate;

public:
  void generateMugger( PlayerCityPtr city, HousePtr house );
  void generateRioter( PlayerCityPtr city, HousePtr house );
  void generateProtestor( PlayerCityPtr city, HousePtr house );
};

SrvcPtr Disorder::create( PlayerCityPtr city )
{
  SrvcPtr ret( new Disorder( city ) );
  ret->drop();

  return SrvcPtr( ret );
}

std::string Disorder::defaultName(){  return CAESARIA_STR_EXT(Disorder);}

Disorder::Disorder( PlayerCityPtr city )
  : Srvc( city, Disorder::defaultName() ), _d( new Impl )
{
  _d->minCrimeLevel = defaultCrimeLevel;
  _d->currentCrimeLevel = 0;
  _d->maxCrimeLevel = 0;
}

void Disorder::timeStep( const unsigned int time )
{
  if( game::Date::isYearChanged() )
  {
    _d->rioterInLastYear = _d->rioterInThisYear;
    _d->rioterInThisYear = 0;
  }

  if( !game::Date::isWeekChanged() )
    return;

  HouseList houses = city::statistic::findh( _city() );

  WalkerList walkers = _city()->walkers( walker::protestor );

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

    int sentiment = _city()->sentiment();
    int randomValue = math::random( crime::maxValue );
    if (sentiment >= minSentiment4protest)
    {
      if ( randomValue >= sentiment + 20 )
      {
        if ( hCrimeLevel > crime::level4protestor )
        {
          _d->generateProtestor( _city(), *it );
        }
      }
    }
    else if ( sentiment >= minSentiment4mugger )
    {
      if ( randomValue >= sentiment + 40 )
      {
        if ( hCrimeLevel >= crime::level4mugger )
        {
          _d->generateMugger( _city(), *it );
        }
        else if ( hCrimeLevel > crime::level4protestor )
        {
          _d->generateProtestor( _city(), *it );
        }
      }
    }
    else if( sentiment < minSentiment4mugger )
    {
      if ( randomValue >= sentiment + 50 )
      {
        if ( hCrimeLevel >= crime::level4rioter )
        {
          _d->generateRioter( _city(), *it );
        }
        else if ( hCrimeLevel >= crime::level4mugger )
        {
          _d->generateMugger( _city(), *it );
        }
        else if ( hCrimeLevel > crime::level4protestor )
        {
          _d->generateProtestor( _city(), *it );
        }
      }
    }
  }
}

std::string Disorder::reason() const
{
  int crimeLevel = math::clamp<int>( _d->currentCrimeLevel / crimeDescLimiter, 0, crimeDescLimiter-1 );
  std::string crimeDesc[ crimeDescLimiter ] = { "##advchief_no_crime##", "##advchief_very_low_crime##", "##advchief_low_crime##",
                                                "##advchief_some_crime##", "##advchief_which_crime##", "##advchief_more_crime##",
                                                "##advchief_simple_crime##", "##advchief_average_crime##", "##advchief_high_crime##",
                                                "##advchief_veryhigh_crime##" };

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

  return troubles.random();
}

unsigned int Disorder::value() const { return _d->currentCrimeLevel; }

VariantMap Disorder::save() const
{
  VariantMap ret;
  VARIANT_SAVE_ANY_D( ret, _d, rioterInLastYear )
  VARIANT_SAVE_ANY_D( ret, _d, rioterInThisYear )
  VARIANT_SAVE_ANY_D( ret, _d, lastMessageDate )
  return ret;
}

void Disorder::load(const VariantMap &stream)
{
  VARIANT_LOAD_ANY_D( _d, rioterInLastYear, stream )
  VARIANT_LOAD_ANY_D( _d, rioterInThisYear, stream )
  VARIANT_LOAD_TIME_D( _d, lastMessageDate, stream )
}

void Disorder::Impl::generateMugger(PlayerCityPtr city, HousePtr house )
{
  house->appendServiceValue( Service::crime, -defaultCrimeLevel / 2 );

  int taxesThisYear = city->funds().getIssueValue( city::Funds::taxIncome );
  int maxMoneyStolen = city->population() / 10;

  if( taxesThisYear > minCityTax4mugger )
  {
    int moneyStolen = taxesThisYear / 4;

    if( moneyStolen > maxMoneyStolen )
      moneyStolen = math::random( maxMoneyStolen );

    events::GameEventPtr e = events::ShowInfobox::create( "##money_stolen_title##", "##money_stolen_text##",
                                                          events::ShowInfobox::send2scribe, "mugging" );
    e->dispatch();

    city->funds().resolveIssue( FundIssue( city::Funds::moneyStolen, -moneyStolen ) );
  }

  currentCrimeLevel++;
}

void Disorder::Impl::generateRioter(PlayerCityPtr city, HousePtr house)
{
  events::GameEventPtr e = events::ShowInfobox::create( "##rioter_in_city_title##", "##rioter_in_city_text##",
                                                        events::ShowInfobox::send2scribe, "spy_riot" );
  e->dispatch();
  rioterInThisYear++;

  RioterPtr protestor = Rioter::create( city );
  protestor->send2City( ptr_cast<Building>( house ) );

  HouseList houses;
  houses << city->overlays();

  foreach( it, houses )
  {
    (*it)->appendServiceValue( Service::crime, -crimedecOnRioterBirth );
  }
}

void Disorder::Impl::generateProtestor(PlayerCityPtr city, HousePtr house)
{
  //ProtestorPtr protestor = Protestor::create( city );
  //protestor->send2City( house );
}

}//end namespace city
