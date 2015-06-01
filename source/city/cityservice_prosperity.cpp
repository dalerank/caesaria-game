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

#include "cityservice_prosperity.hpp"
#include "objects/construction.hpp"
#include "city/statistic.hpp"
#include "core/safetycast.hpp"
#include "core/variant_map.hpp"
#include "core/position.hpp"
#include "city/statistic.hpp"
#include "objects/house.hpp"
#include "objects/house_spec.hpp"
#include "gfx/tile.hpp"
#include "objects/entertainment.hpp"
#include "game/gamedate.hpp"
#include "game/funds.hpp"
#include "world/empire.hpp"
#include "objects/hippodrome.hpp"
#include "objects/constants.hpp"
#include "cityservice_factory.hpp"
#include "cityservice_info.hpp"
#include "city/states.hpp"
#include "config.hpp"

using namespace config;

namespace city
{

REGISTER_SERVICE_IN_FACTORY(ProsperityRating,prosperity)

struct PrInfo
{
  int balance;
  int prosperity;
  int workless;
  bool profit;
  int patricianCount;
  int houseCapTrand;
  int percentPlebs;
  int plebsCount;

  PrInfo()
  {
    memset( this, 0, sizeof(PrInfo) );
  }

  VariantMap save() const
  {
    VariantMap ret;
    VARIANT_SAVE_ANY( ret, balance )
    VARIANT_SAVE_ANY( ret, prosperity )
    VARIANT_SAVE_ANY( ret, workless )
    VARIANT_SAVE_ANY( ret, profit )
    VARIANT_SAVE_ANY( ret, patricianCount )
    VARIANT_SAVE_ANY( ret, plebsCount )
    VARIANT_SAVE_ANY( ret, percentPlebs )
    VARIANT_SAVE_ANY( ret, houseCapTrand )

    return ret;
  }

  void load( const VariantMap& stream )
  {
    VARIANT_LOAD_ANY( balance, stream )
    VARIANT_LOAD_ANY( prosperity, stream )
    VARIANT_LOAD_ANY( workless, stream )
    VARIANT_LOAD_ANY( profit, stream )
    VARIANT_LOAD_ANY( patricianCount, stream )
    VARIANT_LOAD_ANY( plebsCount, stream )
    VARIANT_LOAD_ANY( percentPlebs, stream )
    VARIANT_LOAD_ANY( houseCapTrand, stream )
  }
};

class ProsperityRating::Impl
{
public:
  DateTime lastDate;
  PrInfo prev;
  PrInfo now;
  int prosperityExtend;
  int workersSalary;
};

SrvcPtr ProsperityRating::create( PlayerCityPtr city )
{
  SrvcPtr ret( new ProsperityRating( city ) );
  ret->drop();

  return ret;
}

ProsperityRating::ProsperityRating(PlayerCityPtr city)
  : Srvc( city, defaultName() ), _d( new Impl )
{
  _d->lastDate = game::Date::current();
  _d->prosperityExtend = 0;
  _d->workersSalary = 0;
}

void ProsperityRating::_checkStats()
{
  HouseList houses = statistic::getHouses( _city() );

  int prosperityCap = 0;
  foreach( it, houses)
  {
    HousePtr house = *it;
    prosperityCap += house->spec().prosperity();
    _d->now.patricianCount += house->spec().isPatrician() ? house->habitants().count() : 0;
    _d->now.plebsCount += house->spec().level() < HouseLevel::plebsLevel ? house->habitants().count() : 0;
  }

  if( houses.size() > 0 )
  {
    prosperityCap /= houses.size();
  }

  _d->now.prosperity = math::clamp<int>( prosperityCap, 0, _d->now.prosperity + 2 );
  _d->now.houseCapTrand = _d->now.prosperity - _d->prev.prosperity;
}

void ProsperityRating::timeStep(const unsigned int time )
{
  if( !game::Date::isMonthChanged() )
    return;

  _checkStats();

  if( game::Date::current().year() > _d->lastDate.year() )
  {
    _d->prev.balance = _city()->treasury().getIssueValue( econ::Issue::balance, econ::Treasury::lastYear );
    _d->workersSalary = _city()->treasury().workerSalary();

    _d->lastDate = game::Date::current();

    if( _city()->states().population == 0 )
    {
      _d->now.prosperity = 0;
      _d->prosperityExtend = 0;
      return;
    }

    int currentFunds = _city()->treasury().money();
    _d->now.profit = _d->prev.balance < currentFunds;
    _d->prev.balance = currentFunds;
    _d->prosperityExtend = (_d->now.profit ? prosperity::cityHaveProfitAward : prosperity::penalty );

    bool more10PercentIsPatrician = math::percentage( _d->now.patricianCount, _city()->states().population ) > 10;
    _d->prosperityExtend += (more10PercentIsPatrician ? prosperity::award : 0);

    _d->percentPlebs = math::percentage( _d->now.plebsCount, _city()->states().population );
    _d->prosperityExtend += (_d->percentPlebs < prosperity::normalPlebsInCityPercent ? prosperity::award : 0);

    bool haveHippodrome = !statistic::getObjects<Hippodrome>( _city(), object::hippodrome ).empty();
    _d->prosperityExtend += (haveHippodrome ? prosperity::award : 0);

    _d->now.workless = statistic::getWorklessPercent( _city() );
    bool unemploymentLess5percent = _d->now.workless < prosperity::normalWorklesPercent;
    bool unemploymentMore15percent = _d->now.workless > workless::high;

    _d->prosperityExtend += (unemploymentLess5percent ? prosperity::award : 0);
    _d->prosperityExtend += (unemploymentMore15percent ? prosperity::penalty : 0);
    _d->prosperityExtend += (patricianCount > 0 ? prosperity::award : 0);

    _d->workersSalary = _city()->treasury().workerSalary() - _city()->empire()->workerSalary();
    _d->prosperityExtend += (_d->workersSalary > 0 ? 1 : 0);
    _d->prosperityExtend += (_d->workersSalary < 0 ? prosperity::penalty : 0);
   
    _d->prosperityExtend += (_city()->haveOverduePayment() ? -prosperity::taxBrokenPenalty : 0);
    _d->prosperityExtend += (_city()->isPaysTaxes() ? -prosperity::taxBrokenPenalty : 0);

    unsigned int caesarsHelper = _city()->treasury().getIssueValue( econ::Issue::caesarsHelp, econ::Treasury::thisYear );
    caesarsHelper += _city()->treasury().getIssueValue( econ::Issue::caesarsHelp, econ::Treasury::lastYear );
    if( caesarsHelper > 0 )
      _d->prosperityExtend += -prosperity::caesarHelpCityPenalty;

    _d->prev = _d->now;
  }
}

int ProsperityRating::value() const {  return math::clamp<int>( _d->now.prosperity + _d->prosperityExtend, 0, 100 ); }

int ProsperityRating::getMark(ProsperityRating::Mark type) const
{
  switch( type )
  {
  case housesCap: return _d->now.houseCapTrand;
  case haveProfit: return ( _city()->treasury().money() - _d->prev.balance > 0);
  case worklessPercent: return _d->now.workless;
  case workersSalary: return _d->workersSalary;
  case changeValue: return value() - _d->prev.prosperity;
  case plebsPercent: return _d->percentPlebs;
  }

  return 0;
}

std::string ProsperityRating::defaultName() { return CAESARIA_STR_EXT(ProsperityRating); }

VariantMap ProsperityRating::save() const
{
  VariantMap ret;

  VARIANT_SAVE_CLASS_D( ret, _d, prev )
  VARIANT_SAVE_CLASS_D( ret, _d, now )
  VARIANT_SAVE_ANY_D( ret, _d, prosperityExtend )
  VARIANT_SAVE_ANY_D( ret, _d, workersSalary )
  VARIANT_SAVE_ANY_D( ret, _d, percentPlebs )

  return ret;
}

void ProsperityRating::load(const VariantMap& stream)
{
  VARIANT_LOAD_CLASS_D( _d, prev, stream )
  VARIANT_LOAD_CLASS_D( _d, now, stream )
  VARIANT_LOAD_ANY_D( _d, prosperityExtend, stream )
  VARIANT_LOAD_ANY_D( _d, workersSalary, stream )
      VARIANT_LOAD_ANY_D( _d, percentPlebs, stream )
}

}//end namespace city
