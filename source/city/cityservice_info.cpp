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

#include "cityservice_info.hpp"
#include "city.hpp"
#include "core/safetycast.hpp"
#include "core/position.hpp"
#include "objects/house.hpp"
#include "objects/house_level.hpp"
#include "gfx/tile.hpp"
#include "city/helper.hpp"
#include "good/goodhelper.hpp"
#include "core/utils.hpp"
#include "game/gamedate.hpp"
#include "world/empire.hpp"
#include "funds.hpp"
#include "core/foreach.hpp"
#include "sentiment.hpp"
#include "cityservice_peace.hpp"
#include "core/variant_map.hpp"
#include "statistic.hpp"
#include "cityservice_military.hpp"

using namespace constants;

namespace city
{

CAESARIA_LITERALCONST(lastHistory)
CAESARIA_LITERALCONST(allHistory)
CAESARIA_LITERALCONST(messages)
CAESARIA_LITERALCONST(maxparam)

class Info::Impl
{
public:
  DateTime lastDate;
  Info::History lastYearHistory;
  Info::History allHistory;
  Info::Messages messages;

  Info::MaxParameters maxParams;
};

SrvcPtr Info::create( PlayerCityPtr city )
{
  SrvcPtr ret( new Info( city ) );
  ret->drop();

  return ret;
}

Info::Info( PlayerCityPtr city )
  : Srvc( city, defaultName() ), _d( new Impl )
{
  _d->lastDate = game::Date::current();
  _d->lastYearHistory.resize( 12 );
  _d->maxParams.resize( paramsCount );
}

void Info::timeStep(const unsigned int time )
{
  if( !game::Date::isMonthChanged() )
    return;

  if( game::Date::current().month() != _d->lastDate.month() )
  {
    bool yearChanged = game::Date::current().year() != _d->lastDate.year();
    _d->lastDate = game::Date::current();

    _d->lastYearHistory.erase( _d->lastYearHistory.begin() );
    _d->lastYearHistory.push_back( Parameters() );

    Parameters& last = _d->lastYearHistory.back();
    last.date = _d->lastDate;
    last[ population  ] = _city()->population();
    last[ funds       ] = _city()->funds().treasury();
    last[ taxpayes    ] =  0;//_d->city->getLastMonthTaxpayer();
    last[ foodStock   ] = statistic::getFoodStock( _city() );
    last[ foodMontlyConsumption ] = statistic::getFoodMonthlyConsumption( _city() );
    last[ monthWithFood ] = last[ foodMontlyConsumption ] > 0 ? (last[ foodStock ] / last[ foodMontlyConsumption ]) : 0;

    int foodProducing = statistic::getFoodProducing( _city() );
    int yearlyFoodConsumption = last[ foodMontlyConsumption ] * DateTime::monthsInYear;
    last[ foodKoeff   ] = ( foodProducing - yearlyFoodConsumption > 0 )
                            ? foodProducing / (yearlyFoodConsumption+1)
                            : -(yearlyFoodConsumption / (foodProducing+1) );

    int currentWorkers, rmaxWorkers;
    statistic::getWorkersNumber( _city(), currentWorkers, rmaxWorkers );

    last[ needWorkers ] = rmaxWorkers - currentWorkers;
    last[ maxWorkers  ] = rmaxWorkers;
    last[ workless    ] = statistic::getWorklessPercent( _city() );
    last[ payDiff     ] = _city()->empire()->workerSalary() - _city()->funds().workerSalary();
    last[ tax         ] = _city()->funds().taxRate();
    last[ cityWages   ] = _city()->funds().workerSalary();
    last[ romeWages   ] = _city()->empire()->workerSalary();
    last[ crimeLevel  ] = statistic::getCrimeLevel( _city() );
    last[ favour      ] = _city()->favour();
    last[ prosperity  ] = _city()->prosperity();
    last[ monthWtWar  ] = statistic::months2lastAttack( _city() );
    last[ peace       ] = 0;

    PeacePtr peaceSrvc;
    peaceSrvc << _city()->findService( Peace::defaultName() );
    if( peaceSrvc.isValid() )
    {
      last[ peace ] = peaceSrvc->value();
    }

    MilitaryPtr mil;
    mil << _city()->findService( Military::defaultName() );
    if( mil.isValid() )
    {
      last[ Info::milthreat ] = mil->threatValue();
    }

    Helper helper( _city() );
    HouseList houses = helper.find<House>( objects::house );

    last[ houseNumber ] = 0;
    last[ shackNumber ] = 0;
    foreach( it, houses )
    {
      HousePtr h = *it;

      if( h->habitants().count() > 0 )
      {
        int hLvl = h->spec().level();
        last[ slumNumber ] += ( hLvl == HouseLevel::hovel || hLvl == HouseLevel::tent ? 1 : 0);
        last[ shackNumber ] += ( hLvl >= HouseLevel::shack || hLvl < HouseLevel::hut ? 1 : 0);
        last[ houseNumber ]++;
      }
    }

    SentimentPtr st;
    st << _city()->findService( Sentiment::defaultName() );

    last[ sentiment ] = st->value();

    for( int k=0; k < paramsCount; k++ )
    {
      _d->maxParams[ k ].value = std::max<int>( last[ k ], _d->maxParams[ k ].value );
    }

    if( yearChanged )
    {
      _d->allHistory.push_back( last );
    }
  }
}

Info::Parameters Info::lastParams() const {  return _d->lastYearHistory.empty() ? Parameters() : _d->lastYearHistory.back(); }

Info::Parameters Info::params(unsigned int monthAgo) const
{
  if( _d->lastYearHistory.empty() )
    return Parameters();

  if( monthAgo >= _d->lastYearHistory.size() )
    return _d->lastYearHistory.front();

  return _d->lastYearHistory[ monthAgo ];
}

Info::Parameters Info::yearParams(unsigned int year) const
{
  if( _d->allHistory.empty() )
    return Parameters();

  if( year >= _d->allHistory.size() )
    return _d->allHistory.front();

  return _d->allHistory[ year ];
}

const Info::MaxParameters &Info::maxParams() const
{
  return _d->maxParams;
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
    stepName = utils::format( 0xff, "%02d", step++ );
    currentVm[ stepName ] = (*i).save();
  }
  ret[ lc_lastHistory ] = currentVm;

  step=0;
  VariantMap allVm;
  foreach( i, _d->allHistory )
  {
    stepName = utils::format( 0xff, "%04d", step++ );
    allVm[ stepName ] = (*i).save();
  }
  ret[ lc_allHistory ] = allVm;

  step=0;
  VariantMap messagesVm;
  foreach( i, _d->messages )
  {
    stepName = utils::format( 0xff, "%04d", step++ );
    messagesVm[ stepName ] = (*i).save();
  }
  ret[ lc_messages ] = messagesVm;

  VariantMap maxParamVm;
  for( int k=0; k < paramsCount; k++ )
  {
    VariantList paramVm;
    paramVm << _d->maxParams[ k ].date;
    paramVm << _d->maxParams[ k ].value;
    maxParamVm[ utils::format( 0xff, "%02d", k ) ] = paramVm;
  }
  ret[ lc_maxparam ] = maxParamVm;

  return ret;
}

void Info::load(const VariantMap& stream)
{
  VariantMap currentHistory = stream.get( lc_lastHistory ).toMap();
  foreach( i, currentHistory )
  {
    _d->lastYearHistory.push_back( Parameters() );
    _d->lastYearHistory.back().load( i->second.toList() );
  }

  VariantMap allHistory = stream.get( lc_allHistory ).toMap();
  foreach( i, allHistory )
  {
    _d->allHistory.push_back( Parameters() );
    _d->allHistory.back().load( i->second.toList() );
  }

  VariantMap messages = stream.get( lc_messages ).toMap();
  foreach( i, messages )
  {
    _d->messages.push_back( ScribeMessage() );
    _d->messages.back().load( i->second.toMap() );
  }

  VariantMap maxParamVm = stream.get( lc_maxparam ).toMap();
  _d->maxParams.resize( paramsCount );
  foreach( i, maxParamVm )
  {
    int index = utils::toInt( i->first );
    DateTime date = i->second.toList().get( 0 ).toDateTime();
    int value = i->second.toList().get( 1 ).toInt();
    _d->maxParams[ index ].date = date;
    _d->maxParams[ index ].value = value;
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
CAESARIA_LITERALCONST(opened)
CAESARIA_LITERALCONST(ext)
}

Info::Parameters::Parameters()
{
  resize( paramsCount );

  foreach( i, *this )
    *i = 0;
}

VariantList Info::Parameters::save() const
{
  VariantList vl;
  for( int k=0; k < Info::paramsCount; k++  )
    {
      vl.push_back( (*this)[ k ] );
  }

  return vl;
}

void Info::Parameters::load(const VariantList& stream)
{
  int k=0;
  foreach( it, stream )
  {
    (*this)[ k ] = *it;
    k++;
  }
}

VariantMap Info::ScribeMessage::save() const
{
  VariantMap ret;
  ret[ lc_text ] = Variant( text );
  ret[ lc_title ] = Variant( title );
  ret[ lc_gtype ] = Variant( good::Helper::getTypeName( gtype ) );
  ret[ lc_position ] = position;
  VARIANT_SAVE_ANY( ret, type )
  VARIANT_SAVE_ANY( ret, date )
  ret[ lc_opened ] = opened;
  ret[ lc_ext ] = ext;

  return ret;
}

void Info::ScribeMessage::load(const VariantMap& stream)
{
  text = stream.get( lc_text ).toString();
  title = stream.get( lc_title ).toString();
  gtype = good::Helper::getType( stream.get( lc_gtype ).toString() );
  position = stream.get( lc_position ).toPoint();
  VARIANT_LOAD_ANY( type, stream )
  VARIANT_LOAD_TIME( date, stream )
  opened = stream.get( lc_opened );
  ext = stream.get( lc_ext );
}

}//end namespace city
