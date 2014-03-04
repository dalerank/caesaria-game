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
#include "core/stringhelper.hpp"
#include "game/gamedate.hpp"
#include "world/empire.hpp"
#include "funds.hpp"
#include "statistic.hpp"

namespace city
{

class Info::Impl
{
public:
  typedef std::vector< Info::Parameters > History;
  PlayerCityPtr city;
  DateTime lastDate;
  History params;
};

SrvcPtr Info::create(PlayerCityPtr city )
{
  SrvcPtr ret( new Info( city ) );
  ret->drop();

  return ret;
}

Info::Info( PlayerCityPtr city )
  : Srvc( getDefaultName() ), _d( new Impl )
{
  _d->city = city;
  _d->lastDate = GameDate::current();
  _d->params.resize( 12 );
}

void Info::update( const unsigned int time )
{
  if( time % GameDate::ticksInMonth() != 1 )
    return;

  if( GameDate::current().month() != _d->lastDate.month() )
  {
    _d->lastDate = GameDate::current();

    _d->params.erase( _d->params.begin() );
    _d->params.push_back( Parameters() );

    Parameters& last = _d->params.back();
    last.date = _d->lastDate;
    last.population = _d->city->getPopulation();
    last.funds = _d->city->getFunds().getValue();    
    last.taxpayes =  0;//_d->city->getLastMonthTaxpayer();

    int foodStock = city::Statistic::getFoodStock( _d->city );
    int foodMontlyConsumption = city::Statistic::getFoodMonthlyConsumption( _d->city );
    last.monthWithFood = foodMontlyConsumption > 0 ? (foodStock / foodMontlyConsumption) : 0;

    int foodProducing = city::Statistic::getFoodProducing( _d->city );
    int yearlyFoodConsumption = foodMontlyConsumption * DateTime::monthInYear;
    last.foodKoeff = ( foodProducing - yearlyFoodConsumption > 0 )
                      ? foodProducing / (yearlyFoodConsumption+1)
                      : -1;

    last.needWorkers = city::Statistic::getVacantionsNumber( _d->city );
    last.workless = city::Statistic::getWorklessPercent( _d->city );
    last.payDiff = _d->city->getEmpire()->getWorkerSalary() - _d->city->getFunds().getWorkerSalary();
    last.tax = _d->city->getFunds().getTaxRate();
  }
}

Info::Parameters Info::getLast() const {  return _d->params.empty() ? Parameters() : _d->params.back(); }
std::string Info::getDefaultName(){  return "info"; }

VariantMap Info::save() const
{
  VariantMap ret;

  int step=0;
  for( Impl::History::iterator i=_d->params.begin(); i != _d->params.end(); i++ )
  {
    VariantList step_values;

    const Parameters& p = *i;

    step_values.push_back( p.date );
    step_values.push_back( p.population );
    step_values.push_back( p.funds );
    step_values.push_back( p.tax );
    step_values.push_back( p.taxpayes );
    step_values.push_back( p.monthWithFood );
    step_values.push_back( p.foodKoeff );
    step_values.push_back( p.godsMood );
    step_values.push_back( p.needWorkers );
    step_values.push_back( p.workless );
    step_values.push_back( p.colloseumCoverage );
    step_values.push_back( p.theaterCoverage );
    step_values.push_back( p.entertainment );
    step_values.push_back( p.lifeValue );

    ret[ StringHelper::format( 0xff, "%02d", step ) ] = step_values;
    step++;
  }

  return ret;
}

void Info::load(const VariantMap& stream)
{
  for( VariantMap::const_iterator i=stream.begin(); i != stream.end(); i++ )
  {
    Parameters p;

    VariantList l = i->second.toList();
    p.date = l.get( 0 ).toDateTime();
    p.population = l.get( 1 );
    p.funds = l.get( 2 ) ;
    p.tax = l.get( 3 );
    p.taxpayes = l.get( 4 );
    p.monthWithFood = l.get( 5 );
    p.foodKoeff = l.get( 6 );
    p.godsMood = l.get( 7 );
    p.needWorkers = l.get( 8 );
    p.workless = l.get( 9 );
    p.colloseumCoverage = l.get( 10 );
    p.theaterCoverage = l.get( 11 );
    p.entertainment = l.get( 12 );
    p.lifeValue = l.get( 13 );

    _d->params.push_back( p );
  }

  _d->params.resize( DateTime::monthInYear );
}

}//end namespace city
