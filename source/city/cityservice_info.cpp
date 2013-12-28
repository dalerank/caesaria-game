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

#include "cityservice_info.hpp"
#include "city.hpp"
#include "core/safetycast.hpp"
#include "core/position.hpp"
#include "objects/house.hpp"
#include "objects/house_level.hpp"
#include "gfx/tile.hpp"
#include "game/gamedate.hpp"
#include "funds.hpp"
#include "statistic.hpp"

class CityServiceInfo::Impl
{
public:
  PlayerCityPtr city;
  DateTime lastDate;
  std::vector< CityServiceInfo::Parameters > params;
};

CityServicePtr CityServiceInfo::create(PlayerCityPtr city )
{
  CityServicePtr ret( new CityServiceInfo( city ) );
  ret->drop();

  return ret;
}

CityServiceInfo::CityServiceInfo( PlayerCityPtr city )
  : CityService( getDefaultName() ), _d( new Impl )
{
  _d->city = city;
  _d->lastDate = GameDate::current();
  _d->params.resize( 12 );
}

void CityServiceInfo::update( const unsigned int time )
{
  if( time % 44 != 1 )
    return;

  if( GameDate::current().getMonth() != _d->lastDate.getMonth() )
  {
    _d->lastDate = GameDate::current();

    _d->params.erase( _d->params.begin() );
    _d->params.push_back( Parameters() );

    Parameters& last = _d->params.back();
    last.population = _d->city->getPopulation();
    last.funds = _d->city->getFunds().getValue();    
    last.taxpayes =  0;//_d->city->getLastMonthTaxpayer();

    int foodStock = CityStatistic::getFoodStock( _d->city );
    int foodMontlyConsumption = CityStatistic::getFoodMonthlyConsumption( _d->city );
    last.monthWithFood = foodStock / foodMontlyConsumption;

    int foodProducing = CityStatistic::getFoodProducing( _d->city );
    int yearlyFoodConsumption = foodMontlyConsumption * DateTime::monthInYear;
    last.foodKoeff = ( foodProducing - yearlyFoodConsumption >= 0 )
                      ? foodProducing / yearlyFoodConsumption
                      : -1;

    last.needWorkers = CityStatistic::getVacantionsNumber( _d->city );
    last.workless = CityStatistic::getWorklessPercent( _d->city );
    last.tax = _d->city->getFunds().getTaxRate();
  }
}

CityServiceInfo::Parameters CityServiceInfo::getLast() const
{
  return _d->params.empty()
            ? Parameters()
            : _d->params.back();
}

std::string CityServiceInfo::getDefaultName()
{
  return "info";
}
