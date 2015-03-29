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
#include "objects/house_spec.hpp"
#include "gfx/tile.hpp"
#include "city/helper.hpp"
#include "good/helper.hpp"
#include "core/utils.hpp"
#include "game/gamedate.hpp"
#include "world/empire.hpp"
#include "game/funds.hpp"
#include "core/foreach.hpp"
#include "sentiment.hpp"
#include "cityservice_peace.hpp"
#include "core/variant_map.hpp"
#include "statistic.hpp"
#include "cityservice_military.hpp"
#include "cityservice_factory.hpp"
#include "city/states.hpp"

using namespace constants;

namespace city
{

REGISTER_SERVICE_IN_FACTORY(Info,info)

CAESARIA_LITERALCONST(lastHistory)
CAESARIA_LITERALCONST(allHistory)
CAESARIA_LITERALCONST(maxparam)

class Info::Impl
{
public:
  DateTime lastDate;
  Info::History lastYearHistory;
  Info::History allHistory;
  Info::MaxParameters maxParams;
};

VariantMap Info::History::save() const
{
  VariantMap currentVm;

  int step=0;
  std::string stepName;
  foreach( i, *this )
  {
    stepName = utils::format( 0xff, "%04d", step++ );
    currentVm[ stepName ] = i->save();
  }

  return currentVm;
}

void Info::History::load(const VariantMap &vm)
{
  foreach( i, vm )
  {
    push_back( Parameters() );
    back().load( i->second.toList() );
  }
}

VariantMap Info::MaxParameters::save() const
{
  VariantMap maxParamVm;
  for( int k=0; k < paramsCount; k++ )
  {
    VariantList paramVm;
    paramVm << (*this)[ k ].date;
    paramVm << (*this)[ k ].value;

    maxParamVm[ utils::format( 0xff, "%02d", k ) ] = paramVm;
  }

  return maxParamVm;
}

void Info::MaxParameters::load(const VariantMap &vm)
{
  resize( paramsCount );
  foreach( i, vm )
  {
    int index = utils::toInt( i->first );
    DateTime date = i->second.toList().get( 0 ).toDateTime();
    int value = i->second.toList().get( 1 ).toInt();
    (*this)[ index ].date = date;
    (*this)[ index ].value = value;
  }
}

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
  _d->lastYearHistory.resize( DateTime::monthsInYear );
  _d->maxParams.resize( paramsCount );
}

void Info::timeStep(const unsigned int time )
{
  if( !game::Date::isMonthChanged() )
    return;

  bool isMonthChanged = game::Date::current().month() != _d->lastDate.month();
  if( isMonthChanged )
  {    
    bool isYearChanged = game::Date::current().year() != _d->lastDate.year();
    _d->lastDate = game::Date::current();

    _d->lastYearHistory.erase( _d->lastYearHistory.begin() );
    _d->lastYearHistory.push_back( Parameters() );

    Parameters& last = _d->lastYearHistory.back();
    last.date = _d->lastDate;
    last[ population  ] = _city()->states().population;
    last[ funds       ] = _city()->treasury().money();
    last[ taxpayes    ] =  0;//_d->city->getLastMonthTaxpayer();
    last[ foodStock   ] = statistic::getFoodStock( _city() );
    last[ foodMontlyConsumption ] = statistic::getFoodMonthlyConsumption( _city() );
    last[ monthWithFood ] = last[ foodMontlyConsumption ] > 0 ? (last[ foodStock ] / last[ foodMontlyConsumption ]) : 0;

    int foodProducing = statistic::getFoodProducing( _city() );
    int yearlyFoodConsumption = last[ foodMontlyConsumption ] * DateTime::monthsInYear;
    last[ foodKoeff   ] = ( foodProducing - yearlyFoodConsumption > 0 )
                            ? foodProducing / (yearlyFoodConsumption+1)
                            : -(yearlyFoodConsumption / (foodProducing+1) );

    statistic::WorkersInfo wInfo = statistic::getWorkersNumber( _city() );

    last[ needWorkers ] = wInfo.need - wInfo.current;
    last[ maxWorkers  ] = wInfo.need;
    last[ workless    ] = statistic::getWorklessPercent( _city() );
    last[ payDiff     ] = _city()->empire()->workerSalary() - _city()->treasury().workerSalary();
    last[ tax         ] = _city()->treasury().taxRate();
    last[ cityWages   ] = _city()->treasury().workerSalary();
    last[ romeWages   ] = _city()->empire()->workerSalary();
    last[ crimeLevel  ] = statistic::getCrimeLevel( _city() );
    last[ favour      ] = _city()->favour();
    last[ prosperity  ] = _city()->prosperity();
    last[ monthWtWar  ] = statistic::months2lastAttack( _city() );
    last[ blackHouses ] = statistic::blackHouses( _city() );
    last[ peace       ] = 0;

    PeacePtr peaceSrvc = statistic::finds<Peace>( _city() );
    if( peaceSrvc.isValid() )
    {
      last[ peace ] = peaceSrvc->value();
    }

    MilitaryPtr mil = statistic::finds<Military>( _city() );
    if( mil.isValid() )
    {
      last[ Info::milthreat ] = mil->threatValue();
    }

    HouseList houses = city::statistic::findh( _city() );

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

    SentimentPtr st = statistic::finds<Sentiment>( _city() );

    last[ sentiment ] = st->value();

    for( int k=0; k < paramsCount; k++ )
    {
      _d->maxParams[ k ].value = std::max<int>( last[ k ], _d->maxParams[ k ].value );
    }

    if( isYearChanged )
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

const Info::MaxParameters& Info::maxParams() const { return _d->maxParams; }
const Info::History& Info::history() const { return _d->allHistory; }
std::string Info::defaultName() {  return CAESARIA_STR_EXT(Info); }

VariantMap Info::save() const
{
  VariantMap ret;

  ret[ literals::lastHistory ] = _d->lastYearHistory.save();
  ret[ literals::allHistory ] = _d->allHistory.save();
  ret[ literals::maxparam ] = _d->maxParams.save();

  return ret;
}

void Info::load(const VariantMap& stream)
{
  _d->lastYearHistory.load( stream.get( literals::lastHistory ).toMap());
  _d->allHistory.load( stream.get( literals::allHistory ).toMap() );
  _d->maxParams.load( stream.get( literals::maxparam ).toMap() );

  _d->lastYearHistory.resize( DateTime::monthsInYear );
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

}//end namespace city
