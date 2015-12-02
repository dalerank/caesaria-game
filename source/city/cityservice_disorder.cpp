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
#include "game/funds.hpp"
#include "core/variant_list.hpp"
#include "objects/house.hpp"
#include "walker/rioter.hpp"
#include "core/variant_map.hpp"
#include "game/gamedate.hpp"
#include "events/event.hpp"
#include "walker/mugger.hpp"
#include "events/showinfobox.hpp"
#include "cityservice_factory.hpp"
#include "city/states.hpp"
#include "config.hpp"
#include "walker/protestor.hpp"

using namespace events;
using namespace config;

namespace
{
const int minCityTax4mugger = 20;

const int minSentiment4protest = 60;
const int minSentiment4mugger = 30;

std::string crimeDesc[ crime::maxLevel ] = { "##advchief_no_crime##", "##advchief_very_low_crime##", "##advchief_low_crime##",
                                             "##advchief_some_crime##", "##advchief_which_crime##", "##advchief_more_crime##",
                                             "##advchief_simple_crime##", "##advchief_average_crime##", "##advchief_high_crime##",
                                             "##advchief_veryhigh_crime##" };
}

namespace city
{

REGISTER_SERVICE_IN_FACTORY(Disorder,disorder)

struct CrimeLevel
{
  int minimum;
  int current;
  int maximum;

  VariantList save() const
  {
    VariantList ret;
    ret << minimum << current << maximum;
    return ret;
  }

  void load( const VariantList& stream )
  {
    minimum = stream.get( 0 );
    current = stream.get( 1 );
    maximum = stream.get( 2 );
  }
};

struct CrimelInfo
{
  int thisYear;
  int lastYear;

  void nextYear()
  {
    lastYear = thisYear;
    thisYear = 0;
  }

  VariantList save() const
  {
    VariantList ret;
    ret << lastYear << thisYear;
    return ret;
  }

  void load( const VariantList& stream )
  {
    lastYear = stream.get( 0 );
    thisYear = stream.get( 1 );
  }
};

struct CityCrime
{
  CrimeLevel level;
  CrimelInfo rioters;
  CrimelInfo muggers;
  CrimelInfo protestor;

  VariantMap save() const
  {
    VariantMap ret;
    VARIANT_SAVE_CLASS( ret, rioters )
    VARIANT_SAVE_CLASS( ret, protestor )
    VARIANT_SAVE_CLASS( ret, muggers )
    VARIANT_SAVE_CLASS( ret, level )

    return ret;
  }

  void load(const VariantMap& stream )
  {
    VARIANT_LOAD_CLASS_LIST( rioters, stream )
    VARIANT_LOAD_CLASS_LIST( protestor, stream )
    VARIANT_LOAD_CLASS_LIST( muggers, stream )
    VARIANT_LOAD_CLASS_LIST( level, stream )
  }
};


class Disorder::Impl
{
public:
  CityCrime crime;
  DateTime lastMessageDate;

public:
  void generateMugger( PlayerCityPtr city, HousePtr house );
  void generateRioter( PlayerCityPtr city, HousePtr house );
  void generateProtestor( PlayerCityPtr city, HousePtr house );
  void changeCrimeLevel(PlayerCityPtr city , int delta);
  void weekUpdate( unsigned int time, PlayerCityPtr rcity );
};

std::string Disorder::defaultName(){  return GAME_STR_EXT(Disorder);}

Disorder::Disorder( PlayerCityPtr city )
  : Srvc( city, Disorder::defaultName() ), _d( new Impl )
{
  _d->crime.level.minimum = crime::defaultValue;
  _d->crime.level.current = 0;
  _d->crime.level.maximum = 0;
}

void Disorder::Impl::weekUpdate( unsigned int time, PlayerCityPtr rcity )
{
  HouseList houses = rcity->statistic().houses.find();

  int protestor_n = rcity->statistic().walkers.count( walker::protestor );

  HouseList criminalizedHouse;
  crime.level.current = 0;
  crime.level.maximum = 0;

  for( auto house : houses )
  {
    int currentValue = house->getServiceValue( Service::crime )+1;
    if( currentValue >= crime.level.minimum )
    {
      criminalizedHouse.push_back( house );
    }

    crime.level.current += currentValue;
    crime.level.maximum = std::max<int>( crime.level.maximum, currentValue );
  }

  if( houses.size() > 0 )
    crime.level.current /= houses.size();

  if( (int)criminalizedHouse.size() > protestor_n )
  {
    HousePtr house = criminalizedHouse.random();
    int hCrimeLevel = house->getServiceValue( Service::crime );

    int sentiment = rcity->sentiment();
    int randomValue = math::random( crime::maxValue );
    if (sentiment >= minSentiment4protest )
    {
      if ( randomValue >= sentiment + 20 )
      {
        if ( hCrimeLevel > crime::level4protestor )
        {
          generateProtestor( rcity, house );
        }
      }
    }
    else if ( sentiment >= minSentiment4mugger )
    {
      if ( randomValue >= sentiment + 40 )
      {
        if ( hCrimeLevel >= crime::level4mugger )
        {
          generateMugger( rcity, house );
        }
        else if ( hCrimeLevel > crime::level4protestor )
        {
          generateProtestor( rcity, house );
        }
      }
    }
    else if( sentiment < minSentiment4mugger )
    {
      if ( randomValue >= sentiment + 50 )
      {
        if ( hCrimeLevel >= crime::level4rioter ) { generateRioter( rcity, house ); }
        else if ( hCrimeLevel >= crime::level4mugger ) { generateMugger( rcity, house ); }
        else if ( hCrimeLevel > crime::level4protestor ) { generateProtestor( rcity, house ); }
      }
    }
  }
}

void Disorder::timeStep( const unsigned int time )
{
  if( game::Date::isYearChanged() )
  {
    _d->crime.rioters.nextYear();
    _d->crime.protestor.nextYear();
  }

  if( game::Date::isWeekChanged() )
  {
    _d->weekUpdate( time, _city() );
  }
}

std::string Disorder::reason() const
{
  int limiter = crime::maxValue / crime::maxLevel;
  int currentValue = math::clamp<int>( _d->crime.level.current / limiter, 0, crime::maxLevel-1 );

  StringArray troubles;
  troubles << crimeDesc[ currentValue ];

  if( _d->crime.level.maximum > crime::defaultValue )           { troubles << "##advchief_high_crime_in_district##";  }
  else if( _d->crime.level.maximum > crime::defaultValue / 2 )  { troubles << "##advchief_which_crime_in_district##";  }
  else if( _d->crime.level.maximum > crime::defaultValue / 5 )  { troubles << "##advchief_low_crime##";  }

  return troubles.random();
}

unsigned int Disorder::value() const { return _d->crime.level.current; }

VariantMap Disorder::save() const
{
  VariantMap ret;
  VARIANT_SAVE_CLASS_D( ret, _d, crime )
  VARIANT_SAVE_ANY_D( ret, _d, lastMessageDate )
  return ret;
}

void Disorder::load(const VariantMap &stream)
{
  VARIANT_LOAD_CLASS_D ( _d, crime, stream )
  VARIANT_LOAD_TIME_D( _d, lastMessageDate,      stream )
}

void Disorder::Impl::generateMugger(PlayerCityPtr city, HousePtr house )
{
  int taxesThisYear = city->treasury().getIssueValue( econ::Issue::taxIncome );
  int maxMoneyStolen = city->states().population / 10;

  if( taxesThisYear > minCityTax4mugger )
  {
    int moneyStolen = taxesThisYear / 4;

    if( moneyStolen > maxMoneyStolen )
      moneyStolen = math::random( maxMoneyStolen );

    events::dispatch<ShowInfobox>( "##money_stolen_title##",
                                   "##money_stolen_text##",
                                   true, "mugging" );

    city->treasury().resolveIssue( econ::Issue( econ::Issue::moneyStolen, -moneyStolen ) );
  }

  crime.level.current++;
  crime.muggers.thisYear++;
  //house->appendServiceValue( Service::crime, -crime::defaultValue / 2 );
  changeCrimeLevel( city, -crime::muggerCost );
}

void Disorder::Impl::generateRioter(PlayerCityPtr city, HousePtr house)
{
  events::dispatch<ShowInfobox>( "##rioter_in_city_title##", "##rioter_in_city_text##",
                                 true, "spy_riot" );
  crime.rioters.thisYear++;

  RioterPtr rioter = Walker::create<Rioter>( city );
  rioter->send2City( house.as<Building>() );

  changeCrimeLevel( city, -crime::rioterCost );
}

void Disorder::Impl::generateProtestor(PlayerCityPtr city, HousePtr house)
{
  ProtestorPtr protestor = Walker::create<Protestor>( city );
  protestor->send2City( house.as<Building>() );

  crime.protestor.thisYear++;

  changeCrimeLevel( city, -crime::protestorCost );
}

void Disorder::Impl::changeCrimeLevel(PlayerCityPtr city, int delta )
{
  city->statistic().houses
                   .find()
                   .for_each(
                              [delta](HousePtr house)
                              {
                                house->appendServiceValue( Service::crime, delta );
                              }
                            );
}

}//end namespace city
