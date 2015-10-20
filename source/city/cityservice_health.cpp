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

#include "cityservice_health.hpp"
#include "objects/house.hpp"
#include "statistic.hpp"
#include "statistic.hpp"
#include "game/gamedate.hpp"
#include "statistic.hpp"
#include "events/warningmessage.hpp"
#include "cityservice_factory.hpp"
#include "config.hpp"

using namespace config;
using namespace events;

namespace city
{

REGISTER_SERVICE_IN_FACTORY(HealthCare,healtCare)

static const std::string healthDescription[health::levelNumber]
      = { "##advchief_health_terrible", "##advchief_health_lower", "##advchief_health_low", "##advchief_health_bad",
          "##advchief_health_less", "##advchief_health_middle", "##advchief_health_simple",
          "##advchief_health_high", "##advchief_health_good", "##advchief_health_verygood", "##advchief_health_awesome",
          "##advchief_health_perfect" };

struct ReasonInfo
{
  object::Type type;
  std::string info;
};

static const ReasonInfo reasonsInfo[] = {
  { object::barber, "##advchief_some_need_barber##" },
  { object::baths, "##advchief_some_need_baths##" },
  { object::clinic, "##advchief_some_need_doctors##" },
  { object::hospital, "##advchief_some_need_hospital##" },
  { object::unknown, "" }
};

class HealthCare::Impl
{
public:
  unsigned int value;
  unsigned int avgMinHealth;
  StringArray reasons;

public:
  void updateValue( PlayerCityPtr city );
  void updateReasons(PlayerCityPtr city);
  void showWarningIfNeed();
};

city::SrvcPtr HealthCare::create( PlayerCityPtr city )
{
  SrvcPtr ret( new HealthCare( city ) );
  ret->drop();

  return ret;
}

std::string HealthCare::defaultName() { return CAESARIA_STR_EXT(HealthCare); }

HealthCare::HealthCare( PlayerCityPtr city )
  : Srvc( city, HealthCare::defaultName() ), _d( new Impl )
{
  _d->avgMinHealth = 0;
  _d->value = health::unknownState;
}

void HealthCare::timeStep(const unsigned int time )
{
  if( game::Date::isMonthChanged() )
  {
    _d->updateValue( _city() );
    _d->updateReasons( _city() );
    _d->showWarningIfNeed();
  }
}

unsigned int HealthCare::value() const { return _d->value; }

std::string HealthCare::reason() const
{
  if( _d->value == health::unknownState )
  {
    _d->updateValue( _city() );
    _d->updateReasons( _city() );
  }
  return _d->reasons.random();
}

void HealthCare::Impl::updateValue(PlayerCityPtr city)
{
  HouseList habitable = city->statistic().houses.habitable();

  value = 0;
  avgMinHealth = 100;
  int houseWithBadHealth = 0;
  for( auto house : habitable )
  {
    unsigned int hLvl = house->state( pr::health );
    value += hLvl;
    if( hLvl < health::bad )
    {
      avgMinHealth += hLvl;
      houseWithBadHealth++;
    }
  }

  value /= (habitable.size()+1);
  avgMinHealth /= (houseWithBadHealth+1);
}

void HealthCare::Impl::updateReasons( PlayerCityPtr city )
{
  int lvl = math::clamp<int>( value / (health::maxValue/health::levelNumber), 0, health::levelNumber-1 );
  std::string mainReason = healthDescription[ lvl ];

  int clinics_n = city->statistic().objects.count( object::clinic );

  mainReason += clinics_n > 0 ? "_clinic##" : "##";

  reasons << mainReason;
  if( lvl > health::levelNumber / 3 )
  {
    for( int i=0; reasonsInfo[ i ].type != object::unknown; i++ )
    {
      object::TypeSet availableTypes;
      availableTypes.insert( reasonsInfo[ i ].type );

      HouseList housesWantEvolve = city->statistic().houses.ready4evolve( availableTypes );
      if( housesWantEvolve.size() > 0 )
      {
        reasons << reasonsInfo[i].info;
      }
    }
  }
}

void HealthCare::Impl::showWarningIfNeed()
{
  if( avgMinHealth < health::bad )
  {
    auto event = WarningMessage::create( avgMinHealth < health::terrible
                                             ? "##minimum_health_terrible##"
                                             : "##minimum_health_bad##", 2 );
    event->dispatch();
  }
}

}//end namespace city
