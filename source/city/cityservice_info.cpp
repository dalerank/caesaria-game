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
#include "core/foreach.hpp"
#include "statistic.hpp"

namespace city
{

class Info::Impl
{
public:
  DateTime lastDate;
  Info::History lastYearHistory;
  Info::History allHistory;
  Info::Messages messages;
};

SrvcPtr Info::create(PlayerCityPtr city )
{
  SrvcPtr ret( new Info( city ) );
  ret->drop();

  return ret;
}

Info::Info( PlayerCityPtr city )
  : Srvc( *city.object(), getDefaultName() ), _d( new Impl )
{
  _d->lastDate = GameDate::current();
  _d->lastYearHistory.resize( 12 );
}

void Info::update( const unsigned int time )
{
  if( !GameDate::isMonthChanged() )
    return;

  if( GameDate::current().month() != _d->lastDate.month() )
  {
    bool yearChanged = GameDate::current().year() != _d->lastDate.year();
    _d->lastDate = GameDate::current();

    _d->lastYearHistory.erase( _d->lastYearHistory.begin() );
    _d->lastYearHistory.push_back( Parameters() );

    Parameters& last = _d->lastYearHistory.back();
    last.date = _d->lastDate;
    last.population = _city.population();
    last.funds = _city.funds().treasury();
    last.taxpayes =  0;//_d->city->getLastMonthTaxpayer();

    int foodStock = city::Statistic::getFoodStock( &_city );
    int foodMontlyConsumption = city::Statistic::getFoodMonthlyConsumption( &_city );
    last.monthWithFood = foodMontlyConsumption > 0 ? (foodStock / foodMontlyConsumption) : 0;

    int foodProducing = city::Statistic::getFoodProducing( &_city );
    int yearlyFoodConsumption = foodMontlyConsumption * DateTime::monthsInYear;
    last.foodKoeff = ( foodProducing - yearlyFoodConsumption > 0 )
                      ? foodProducing / (yearlyFoodConsumption+1)
                      : -1;

    int currentWorkers, maxWorkers;
    city::Statistic::getWorkersNumber( &_city, currentWorkers, maxWorkers );

    last.needWorkers = maxWorkers - currentWorkers;
    last.maxWorkers = maxWorkers;
    last.workless = city::Statistic::getWorklessPercent( &_city );
    last.payDiff = _city.empire()->getWorkerSalary() - _city.funds().workerSalary();
    last.tax = _city.funds().taxRate();
    last.cityWages = _city.funds().workerSalary();
    last.romeWages = _city.empire()->getWorkerSalary();
    last.crimeLevel = city::Statistic::getCrimeLevel( &_city );

    if( yearChanged )
    {
      _d->allHistory.push_back( last );
    }
  }
}

Info::Parameters Info::getLast() const {  return _d->lastYearHistory.empty() ? Parameters() : _d->lastYearHistory.back(); }

const Info::History& Info ::getHistory() const { return _d->allHistory; }

std::string Info::getDefaultName(){  return CAESARIA_STR_EXT(Info); }

VariantMap Info::save() const
{
  VariantMap ret;

  int step=0;
  std::string stepName;
  VariantMap currentVm;
  foreach( i, _d->lastYearHistory )
  {
    stepName = StringHelper::format( 0xff, "%02d", step++ );
    currentVm[ stepName ] = (*i).save();
  }
  ret[ "lastHistory" ] = currentVm;

  step=0;
  VariantMap allVm;
  foreach( i, _d->allHistory )
  {
    stepName = StringHelper::format( 0xff, "%04d", step++ );
    allVm[ stepName ] = (*i).save();
  }
  ret[ "allHistory" ] = allVm;

  return ret;
}

void Info::load(const VariantMap& stream)
{
  VariantMap currentHistory = stream.get( "lastHistory" ).toMap();
  foreach( i, currentHistory )
  {
    Parameters p;
    p.load( i->second.toMap() );
    _d->lastYearHistory.push_back( p );
  }

  VariantMap allHistory = stream.get( "allHistory" ).toMap();
  foreach( i, allHistory )
  {
    Parameters p;
    p.load( i->second.toMap() );
    _d->allHistory.push_back( p );
  }

  _d->lastYearHistory.resize( DateTime::monthsInYear );
}

const Info::Messages& Info::messages() const { return _d->messages; }

const Info::ScribeMessage& Info::getMessage(int index) const
{
  static ScribeMessage invalidMessage;
  Messages::iterator it = _d->messages.begin();
  std::advance( it, index );
  if( it != _d->messages.end() )
    return *it;

  return invalidMessage;
}

void Info::changeMessage(int index, ScribeMessage& message)
{
  Messages::iterator it = _d->messages.begin();
  std::advance( it, index );
  if( it != _d->messages.end() )
    *it = message;
}

void Info::removeMessage(int index)
{
  Messages::iterator it = _d->messages.begin();
  std::advance( it, index );
  if( it != _d->messages.end() )
    _d->messages.erase( it );
}

void Info::addMessage(const Info::ScribeMessage& message)
{
  _d->messages.push_front( message );
}

namespace {
CAESARIA_LITERALCONST(date)
CAESARIA_LITERALCONST(population)
CAESARIA_LITERALCONST(funds)
CAESARIA_LITERALCONST(tax)
CAESARIA_LITERALCONST(taxpayes)
CAESARIA_LITERALCONST(monthWithFood)

CAESARIA_LITERALCONST(foodKoeff)
CAESARIA_LITERALCONST(godsMood)
CAESARIA_LITERALCONST(needWorkers)
CAESARIA_LITERALCONST(colloseumCoverage)
CAESARIA_LITERALCONST(theaterCoverage)
CAESARIA_LITERALCONST(workless)
CAESARIA_LITERALCONST(entertainment)
CAESARIA_LITERALCONST(lifeValue)
}

VariantMap Info::Parameters::save() const
{
  VariantMap ret;
  ret[ lc_date ] = date;
  ret[ lc_population ] = population;
  ret[ lc_funds ] = funds;
  ret[ lc_tax ] = tax;
  ret[ lc_taxpayes ] = taxpayes;
  ret[ lc_monthWithFood ] = monthWithFood;
  ret[ lc_foodKoeff ] = foodKoeff;
  ret[ lc_godsMood ] = godsMood;
  ret[ lc_needWorkers ] = needWorkers;
  ret[ lc_workless ] = workless;
  ret[ lc_colloseumCoverage ] = colloseumCoverage;
  ret[ lc_theaterCoverage ] = theaterCoverage;
  ret[ lc_entertainment ] = entertainment;
  ret[ lc_lifeValue ] = lifeValue;

  return ret;
}

void Info::Parameters::load(const VariantMap& stream)
{
  date = stream.get( lc_date ).toDateTime();
  population = stream.get( lc_population );
  funds = stream.get( lc_funds ) ;
  tax = stream.get( lc_tax );
  taxpayes = stream.get( lc_taxpayes );
  monthWithFood = stream.get( lc_monthWithFood );
  foodKoeff = stream.get( lc_foodKoeff );
  godsMood = stream.get( lc_godsMood );
  needWorkers = stream.get( lc_needWorkers );
  workless = stream.get( lc_workless );
  colloseumCoverage = stream.get( lc_colloseumCoverage );
  theaterCoverage = stream.get( lc_theaterCoverage );
  entertainment = stream.get( lc_entertainment );
  lifeValue = stream.get( lc_lifeValue );
}

}//end namespace city
