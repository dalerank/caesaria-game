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

namespace city
{

REGISTER_SERVICE_IN_FACTORY(Info,info)

class Info::Impl
{
public:
  DateTime lastDate;
  Info::History lastHistory;
  Info::History allHistory;
  Info::MaxParameters maxparam;
};

VariantMap Info::History::save() const
{
  VariantMap currentVm;

  int step=0;
  std::string stepName;
  for( auto item : *this )
  {
    stepName = utils::format( 0xff, "%04d", step++ );
    currentVm[ stepName ] = item.save();
  }

  return currentVm;
}

void Info::History::load(const VariantMap &vm)
{
  for( auto i : vm )
  {
    push_back( Parameters() );
    back().load( i.second.toList() );
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
  for( auto item : vm )
  {
    int index = utils::toInt( item.first );
    DateTime date = item.second.toList().get( 0 ).toDateTime();
    int value = item.second.toList().get( 1 ).toInt();
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
  _d->lastHistory.resize( DateTime::monthsInYear );
  _d->maxparam.resize( paramsCount );
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

    _d->lastHistory.erase( _d->lastHistory.begin() );
    _d->lastHistory.push_back( Parameters() );

    Parameters& last = _d->lastHistory.back();
    last.date = _d->lastDate;
    last[ population  ] = _city()->states().population;
    last[ funds       ] = _city()->treasury().money();
    last[ taxpayes    ] =  0;//_d->city->getLastMonthTaxpayer();
    last[ foodStock   ] = _city()->statistic().food.inGranaries();
    last[ foodMontlyConsumption ] = _city()->statistic().food.monthlyConsumption();
    last[ monthWithFood ] = last[ foodMontlyConsumption ] > 0 ? (last[ foodStock ] / last[ foodMontlyConsumption ]) : 0;

    int foodProducing = _city()->statistic().food.possibleProducing();
    int yearlyFoodConsumption = last[ foodMontlyConsumption ] * DateTime::monthsInYear;
    last[ foodKoeff   ] = ( foodProducing - yearlyFoodConsumption > 0 )
                            ? foodProducing / (yearlyFoodConsumption+1)
                            : -(yearlyFoodConsumption / (foodProducing+1) );

    Statistic::WorkersInfo wInfo = _city()->statistic().workers.details();

    last[ needWorkers ] = wInfo.need - wInfo.current;
    last[ maxWorkers  ] = wInfo.need;
    last[ workless    ] = _city()->statistic().workers.worklessPercent();
    last[ payDiff     ] = _city()->empire()->workerSalary() - _city()->treasury().workerSalary();
    last[ tax         ] = _city()->treasury().taxRate();
    last[ cityWages   ] = _city()->treasury().workerSalary();
    last[ romeWages   ] = _city()->empire()->workerSalary();
    last[ crimeLevel  ] = _city()->statistic().crime.level();
    last[ favour      ] = _city()->favour();
    last[ prosperity  ] = _city()->prosperity();
    last[ monthWtWar  ] = _city()->statistic().military.months2lastAttack();
    last[ blackHouses ] = _city()->statistic().houses.terribleNumber();
    last[ peace       ] = 0;

    PeacePtr peaceSrvc = _city()->statistic().services.find<Peace>();
    if( peaceSrvc.isValid() )
    {
      last[ peace ] = peaceSrvc->value();
    }

    MilitaryPtr mil = _city()->statistic().services.find<Military>();
    if( mil.isValid() )
    {
      last[ Info::milthreat ] = mil->threatValue();
    }

    HouseList houses = _city()->statistic().houses.find();

    last[ houseNumber ] = 0;
    last[ shackNumber ] = 0;
    for( auto house : houses )
    {
      if( house->habitants().count() > 0 )
      {
        int hLvl = house->spec().level();
        last[ slumNumber ] += ( hLvl == HouseLevel::hovel || hLvl == HouseLevel::tent ? 1 : 0);
        last[ shackNumber ] += ( hLvl >= HouseLevel::shack || hLvl < HouseLevel::hut ? 1 : 0);
        last[ houseNumber ]++;
      }
    }

    SentimentPtr sentimentSrvc = _city()->statistic().services.find<Sentiment>();

    if( sentimentSrvc.isValid())
      last[ sentiment ] = sentimentSrvc->value();

    for( int k=0; k < paramsCount; k++ )
    {
      _d->maxparam[ k ].value = std::max<int>( last[ k ], _d->maxparam[ k ].value );
    }

    if( isYearChanged )
    {
      _d->allHistory.push_back( last );
    }
  }
}

Info::Parameters Info::lastParams() const { return _d->lastHistory.empty() ? Parameters() : _d->lastHistory.back(); }

Info::Parameters Info::params(unsigned int monthAgo) const
{
  if( _d->lastHistory.empty() )
    return Parameters();

  if( monthAgo >= _d->lastHistory.size() )
    return _d->lastHistory.front();

  return _d->lastHistory[ monthAgo ];
}

Info::Parameters Info::yearParams(unsigned int year) const
{
  if( _d->allHistory.empty() )
    return Parameters();

  if( year >= _d->allHistory.size() )
    return _d->allHistory.front();

  return _d->allHistory[ year ];
}

const Info::MaxParameters& Info::maxParams() const { return _d->maxparam; }
const Info::History& Info::history() const { return _d->allHistory; }
std::string Info::defaultName() {  return CAESARIA_STR_EXT(Info); }

VariantMap Info::save() const
{
  VariantMap ret;

  VARIANT_SAVE_CLASS_D( ret, _d, lastHistory )
  VARIANT_SAVE_CLASS_D( ret, _d, allHistory )
  VARIANT_SAVE_CLASS_D( ret, _d, maxparam )

  return ret;
}

void Info::load(const VariantMap& stream)
{
  VARIANT_LOAD_CLASS_D( _d, lastHistory, stream )
  VARIANT_LOAD_CLASS_D( _d, allHistory, stream )
  VARIANT_LOAD_CLASS_D( _d, maxparam, stream )

  if( _d->lastHistory.size() > DateTime::monthsInYear )
  {
    Info::History::iterator oldHistoryStart = _d->lastHistory.begin();
    Info::History::iterator oldHistoryEnd = _d->lastHistory.begin() + _d->lastHistory.size() - DateTime::monthsInYear;
    _d->lastHistory.erase( oldHistoryStart, oldHistoryEnd );
  }
}

Info::Parameters::Parameters()
{
  resize( paramsCount, 0 );
}

Info::Parameters::Parameters(const Info::Parameters& other)
{
  resize( paramsCount );
  for( unsigned int i=0; i < other.size(); i++ )
    (*this)[ i ] = other[ i ];
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
  for( auto it : stream )
  {
    (*this)[ k ] = it;
    k++;
  }
}

}//end namespace city
