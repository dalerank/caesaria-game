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

class HealthCare::Impl
{
public:
  unsigned int value;
  unsigned int avgMinHealth;
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
  _d->value = 0;
}

void HealthCare::timeStep(const unsigned int time )
{
  if( game::Date::isMonthChanged() )
  {
    HouseList houses = city::statistic::getHouses( _city() );

    _d->value = 0;
    _d->avgMinHealth = 100;
    int houseWithBadHealth = 0;
    foreach( house, houses )
    {
      unsigned int hLvl = (*house)->state( pr::health );
      if( (*house)->habitants().count() > 0 )
      {
        _d->value += hLvl;
        if( hLvl < health::bad )
        {
          _d->avgMinHealth += hLvl;
          houseWithBadHealth++;
        }
      }           
    }

    _d->value /= (houses.size()+1);
    _d->avgMinHealth /= (houseWithBadHealth+1);

    if( _d->avgMinHealth < health::bad )
    {
      GameEventPtr e = WarningMessage::create( _d->avgMinHealth < health::terrible
                                               ? "##minimum_health_terrible##"
                                               : "##minimum_health_bad##", 2 );
      e->dispatch();
    }
  }
}

unsigned int HealthCare::value() const { return _d->value; }

std::string HealthCare::reason() const
{
  StringArray reasons;  

  int lvl = math::clamp<int>( _d->value / (health::maxValue/health::levelNumber), 0, health::levelNumber-1 );
  std::string mainReason = healthDescription[ lvl ];

  BuildingList clinics = city::statistic::getObjects<Building>( _city(), object::clinic );

  mainReason += clinics.size() > 0 ? "_clinic##" : "##";

  reasons << mainReason;
  if( lvl > health::levelNumber / 3 )
  {
    object::Type avTypes[] = { object::barber, object::baths, object::clinic, object::hospital, object::unknown };
    std::string avReasons[] = { "##advchief_some_need_barber##", "##advchief_some_need_baths##",
                                "##advchief_some_need_doctors##", "##advchief_some_need_hospital##",
                                "" };

    for( int i=0; avTypes[ i ] != object::unknown; i++ )
    {
      object::TypeSet availableTypes;
      availableTypes.insert( avTypes[ i ] );

      HouseList housesWantEvolve = statistic::getEvolveHouseReadyBy( _city(), availableTypes );
      if( housesWantEvolve.size() > 0 )
      {
        reasons << avReasons[i];
      }
    }
  }

  return reasons.random();
}

}//end namespace city
