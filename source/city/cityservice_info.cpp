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
#include "city/helper.hpp"
#include "good/goodhelper.hpp"
#include "core/stringhelper.hpp"
#include "game/gamedate.hpp"
#include "world/empire.hpp"
#include "funds.hpp"
#include "core/foreach.hpp"
#include "sentiment.hpp"
#include "cityservice_peace.hpp"
#include "statistic.hpp"

using namespace constants;

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

    last.foodStock = city::Statistic::getFoodStock( &_city );
    last.foodMontlyConsumption = city::Statistic::getFoodMonthlyConsumption( &_city );
    last.monthWithFood = last.foodMontlyConsumption > 0 ? (last.foodStock / last.foodMontlyConsumption) : 0;

    int foodProducing = city::Statistic::getFoodProducing( &_city );
    int yearlyFoodConsumption = last.foodMontlyConsumption * DateTime::monthsInYear;
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

    last.monthWithourWar = city::Statistic::months2lastAttack( &_city );
    last.peace = 0;

    PeacePtr peaceSrvc;
    peaceSrvc << _city.findService( Peace::getDefaultName() );
    if( peaceSrvc.isValid() )
    {
      last.peace = peaceSrvc->value();
    }

    Helper helper( &_city );
    HouseList houses = helper.find<House>( building::house );

    last.houseNumber = 0;
    last.shackNumber = 0;
    foreach( it, houses )
    {
      HousePtr h = *it;

      if( h->habitants().count() > 0 )
      {
        int hLvl = h->spec().level();
        last.slumNumber += ( hLvl == HouseLevel::smallHovel || hLvl == HouseLevel::bigTent ? 1 : 0);
        last.shackNumber += ( hLvl >= HouseLevel::smallHut || hLvl < HouseLevel::bigHut ? 1 : 0);
        last.houseNumber++;
      }
    }

    SentimentPtr st;
    st << _city.findService( Sentiment::defaultName() );

    last.sentiment = st->value();

    if( yearChanged )
    {
      _d->allHistory.push_back( last );
    }
  }
}

Info::Parameters Info::lastParams() const {  return _d->lastYearHistory.empty() ? Parameters() : _d->lastYearHistory.back(); }

Info::Parameters Info::params(int monthAgo) const
{
  if( _d->lastYearHistory.empty() )
    return Parameters();

  if( monthAgo >= _d->lastYearHistory.size() )
    return _d->lastYearHistory.front();

  return _d->lastYearHistory[ monthAgo ];
}

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
CAESARIA_LITERALCONST(text)
CAESARIA_LITERALCONST(title)
CAESARIA_LITERALCONST(gtype)
CAESARIA_LITERALCONST(position)
CAESARIA_LITERALCONST(type)
CAESARIA_LITERALCONST(opened)
CAESARIA_LITERALCONST(ext)
}

VariantMap Info::Parameters::save() const
{
  VariantMap ret;
  VARIANT_SAVE_ANY( ret, date )
  VARIANT_SAVE_ANY( ret, population )
  VARIANT_SAVE_ANY( ret, funds )
  VARIANT_SAVE_ANY( ret, tax )
  VARIANT_SAVE_ANY( ret, taxpayes )
  VARIANT_SAVE_ANY( ret, monthWithFood )
  VARIANT_SAVE_ANY( ret, foodKoeff )
  VARIANT_SAVE_ANY( ret, godsMood )
  VARIANT_SAVE_ANY( ret, needWorkers )
  VARIANT_SAVE_ANY( ret, workless )
  VARIANT_SAVE_ANY( ret, colloseumCoverage )
  VARIANT_SAVE_ANY( ret, theaterCoverage )
  VARIANT_SAVE_ANY( ret, entertainment )
  VARIANT_SAVE_ANY( ret, lifeValue )
  VARIANT_SAVE_ANY( ret, education)
  VARIANT_SAVE_ANY( ret, payDiff )
  VARIANT_SAVE_ANY( ret, monthWithourWar )
  VARIANT_SAVE_ANY( ret, cityWages )
  VARIANT_SAVE_ANY( ret, romeWages )
  VARIANT_SAVE_ANY( ret, maxWorkers )
  VARIANT_SAVE_ANY( ret, crimeLevel )
  VARIANT_SAVE_ANY( ret, peace )
  VARIANT_SAVE_ANY( ret, houseNumber )
  VARIANT_SAVE_ANY( ret, slumNumber )
  VARIANT_SAVE_ANY( ret, shackNumber )
  VARIANT_SAVE_ANY( ret, sentiment )
  VARIANT_SAVE_ANY( ret, foodStock )
  VARIANT_SAVE_ANY( ret, foodMontlyConsumption )

  return ret;
}

void Info::Parameters::load(const VariantMap& stream)
{
  VARIANT_LOAD_TIME( date, stream )
  VARIANT_LOAD_ANY( population, stream )
  VARIANT_LOAD_ANY( funds, stream )
  VARIANT_LOAD_ANY( tax, stream )
  VARIANT_LOAD_ANY( taxpayes, stream )
  VARIANT_LOAD_ANY( monthWithFood, stream )
  VARIANT_LOAD_ANY( foodKoeff, stream )
  VARIANT_LOAD_ANY( godsMood, stream )
  VARIANT_LOAD_ANY( needWorkers, stream )
  VARIANT_LOAD_ANY( workless, stream )
  VARIANT_LOAD_ANY( colloseumCoverage, stream )
  VARIANT_LOAD_ANY( theaterCoverage, stream )
  VARIANT_LOAD_ANY( entertainment, stream )
  VARIANT_LOAD_ANY( lifeValue, stream )
  VARIANT_LOAD_ANY( education, stream )
  VARIANT_LOAD_ANY( payDiff, stream )
  VARIANT_LOAD_ANY( monthWithourWar, stream )
  VARIANT_LOAD_ANY( cityWages, stream )
  VARIANT_LOAD_ANY( romeWages, stream )
  VARIANT_LOAD_ANY( maxWorkers, stream )
  VARIANT_LOAD_ANY( crimeLevel, stream )
  VARIANT_LOAD_ANY( peace, stream )
  VARIANT_LOAD_ANY( houseNumber, stream )
  VARIANT_LOAD_ANY( shackNumber, stream )
  VARIANT_LOAD_ANY( slumNumber, stream )
  VARIANT_LOAD_ANY( sentiment, stream )
  VARIANT_LOAD_ANY( foodStock, stream )
  VARIANT_LOAD_ANY( foodMontlyConsumption, stream )
}

VariantMap Info::ScribeMessage::save() const
{
  VariantMap ret;
  ret[ lc_text ] = Variant( text );
  ret[ lc_title ] = Variant( title );
  ret[ lc_gtype ] = Variant( GoodHelper::getTypeName( gtype ) );
  ret[ lc_position ] = position;
  ret[ lc_type ] = type;
  VARIANT_SAVE_ANY( ret, date )
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
  VARIANT_LOAD_TIME( date, stream )
  opened = stream.get( lc_opened );
  ext = stream.get( lc_ext );
}

}//end namespace city
