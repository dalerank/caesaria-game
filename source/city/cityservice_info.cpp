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
#include "good/goodhelper.hpp"
#include "core/stringhelper.hpp"
#include "game/gamedate.hpp"
#include "world/empire.hpp"
#include "funds.hpp"
#include "core/foreach.hpp"
#include "cityservice_peace.hpp"
#include "statistic.hpp"

namespace city
{

CAESARIA_LITERALCONST(lastHistory)
CAESARIA_LITERALCONST(allHistory)
CAESARIA_LITERALCONST(messages)

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
  : Srvc( *city.object(), defaultName() ), _d( new Impl )
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
                      : -(yearlyFoodConsumption / (foodProducing+1) );

    int currentWorkers, maxWorkers;
    city::Statistic::getWorkersNumber( &_city, currentWorkers, maxWorkers );

    last.needWorkers = maxWorkers - currentWorkers;
    last.maxWorkers = maxWorkers;
    last.workless = city::Statistic::getWorklessPercent( &_city );
    last.payDiff = _city.empire()->workerSalary() - _city.funds().workerSalary();
    last.tax = _city.funds().taxRate();
    last.cityWages = _city.funds().workerSalary();
    last.romeWages = _city.empire()->workerSalary();
    last.crimeLevel = city::Statistic::getCrimeLevel( &_city );    
    last.peace = 0;

    PeacePtr peaceSrvc;
    peaceSrvc << _city.findService( Peace::getDefaultName() );
    if( peaceSrvc.isValid() )
    {
      last.peace = peaceSrvc->value();
    }

    if( yearChanged )
    {
      _d->allHistory.push_back( last );
    }
  }
}

Info::Parameters Info::lastParams() const {  return _d->lastYearHistory.empty() ? Parameters() : _d->lastYearHistory.back(); }

const Info::History& Info ::history() const { return _d->allHistory; }

std::string Info::defaultName(){  return CAESARIA_STR_EXT(Info); }

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
  ret[ lc_lastHistory ] = currentVm;

  step=0;
  VariantMap allVm;
  foreach( i, _d->allHistory )
  {
    stepName = StringHelper::format( 0xff, "%04d", step++ );
    allVm[ stepName ] = (*i).save();
  }
  ret[ lc_allHistory ] = allVm;

  step=0;
  VariantMap messagesVm;
  foreach( i, _d->messages )
  {
    stepName = StringHelper::format( 0xff, "%04d", step++ );
    messagesVm[ stepName ] = (*i).save();
  }
  ret[ lc_messages ] = messagesVm;

  return ret;
}

void Info::load(const VariantMap& stream)
{
  VariantMap currentHistory = stream.get( lc_lastHistory ).toMap();
  foreach( i, currentHistory )
  {
    _d->lastYearHistory.push_back( Parameters() );
    _d->lastYearHistory.back().load( i->second.toMap() );
  }

  VariantMap allHistory = stream.get( lc_allHistory ).toMap();
  foreach( i, allHistory )
  {
    _d->allHistory.push_back( Parameters() );
    _d->allHistory.back().load( i->second.toMap() );
  }

  VariantMap messages= stream.get( lc_messages ).toMap();
  foreach( i, messages )
  {
    _d->messages.push_back( ScribeMessage() );
    _d->messages.back().load( i->second.toMap() );
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

CAESARIA_LITERALCONST(text)
CAESARIA_LITERALCONST(title)
CAESARIA_LITERALCONST(gtype)
CAESARIA_LITERALCONST(position)
CAESARIA_LITERALCONST(type)
CAESARIA_LITERALCONST(date)
CAESARIA_LITERALCONST(opened)
CAESARIA_LITERALCONST(ext)
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

VariantMap Info::ScribeMessage::save() const
{
  VariantMap ret;
  ret[ lc_text ] = Variant( text );
  ret[ lc_title ] = Variant( title );
  ret[ lc_gtype ] = Variant( GoodHelper::getTypeName( gtype ) );
  ret[ lc_position ] = position;
  ret[ lc_type ] = type;
  ret[ lc_date ] = date;
  ret[ lc_opened ] = opened;
  ret[ lc_ext ] = ext;

  return ret;
}

void Info::ScribeMessage::load(const VariantMap& stream)
{
  text = stream.get( lc_text ).toString();
  title = stream.get( lc_title ).toString();
  gtype = GoodHelper::getType( stream.get( lc_gtype ).toString() );
  position = stream.get( lc_position ).toPoint();
  type = stream.get( lc_type );
  date = stream.get( lc_date ).toDateTime();
  opened = stream.get( lc_opened );
  ext = stream.get( lc_ext );
}

}//end namespace city
