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
#include "helper.hpp"
#include "game/gamedate.hpp"
#include "statistic.hpp"

using namespace constants;

namespace city
{

namespace {
const int maxDescriptionLevel = 12;
}

class HealthCare::Impl
{
public:
  unsigned int value;
  unsigned int minHealthLevel;
};

city::SrvcPtr HealthCare::create()
{
  SrvcPtr ret( new HealthCare() );
  ret->drop();

  return ret;
}

std::string HealthCare::defaultName()
{
  return CAESARIA_STR_EXT(HealthCare);
}

HealthCare::HealthCare()
  : city::Srvc( HealthCare::defaultName() ), _d( new Impl )
{
  _d->minHealthLevel = 0;
  _d->value = 0;
}

void HealthCare::timeStep( PlayerCityPtr city, const unsigned int time )
{
  if( GameDate::isMonthChanged() )
  {
    Helper helper( city );
    HouseList houses = helper.find<House>( building::house );

    _d->value = 0;
    _d->minHealthLevel = 0;
    foreach( house, houses )
    {
      unsigned int hLvl = (*house)->state( (Construction::Param)House::health );
      _d->value = ( _d->value + hLvl ) / 2;
    }
  }
  //unsigned int vacantPop=0;
}

unsigned int HealthCare::value() const { return _d->value; }

std::string HealthCare::reason( PlayerCityPtr city ) const
{
  StringArray reasons;

  std::string healthDescription[maxDescriptionLevel]
      = { "##advchief_health_terrible", "##advchief_health_lower", "##advchief_health_low", "##advchief_health_bad",
          "##advchief_health_less", "##advchief_health_middle", "##advchief_health_simple",
          "##advchief_health_high", "##advchief_health_good", "##advchief_health_verygood", "##advchief_health_awesome",
          "##advchief_health_perfect" };

  int lvl = math::clamp<int>( _d->value / (100/maxDescriptionLevel), 0, maxDescriptionLevel-1 );
  std::string mainReason = healthDescription[ lvl ];

  Helper helper( city );
  BuildingList clinics = helper.find<Building>( building::doctor );

  mainReason += clinics.size() > 0 ? "_clinic##" : "##";

  reasons << mainReason;
  if( lvl > maxDescriptionLevel / 3 )
  {
    int avTypes[] = { building::barber, building::baths, building::doctor, building::hospital, building::unknown };
    std::string avReasons[] = { "##advchief_some_need_barber##", "##advchief_some_need_baths##",
                                "##advchief_some_need_doctors##", "##advchief_some_need_hospital##",
                                "" };

    for( int i=0; avTypes[ i ] != building::unknown; i++ )
    {
      std::set<int> availableTypes;
      availableTypes.insert( avTypes[ i ] );

      HouseList houses = Statistic::getEvolveHouseReadyBy( city, availableTypes );
      if( houses.size() > 0 )
      {
        reasons << avReasons[i];
      }
    }
  }

  return reasons.random();
}

}//end namespace city
