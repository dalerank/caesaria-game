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

using namespace  constants;
using namespace config;

namespace city
{

REGISTER_SERVICE_IN_FACTORY(ProsperityRating,prosperity)

class ProsperityRating::Impl
{
public:
  DateTime lastDate;
  int prosperity;
  int houseCapTrand;
  int prosperityExtend;
  bool makeProfit;
  int lastYearBalance;
  int worklessPercent;
  int lastYearProsperity;
  int workersSalary;
  int percentPlebs;
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
  _d->prosperity = 0;
  _d->houseCapTrand = 0;
  _d->prosperityExtend = 0;
  _d->makeProfit = false;
  _d->lastYearBalance = 0;
  _d->worklessPercent = 0;
  _d->workersSalary = 0;
  _d->lastYearProsperity = 0;
  _d->percentPlebs = 0;
}

void ProsperityRating::timeStep(const unsigned int time )
{
  if( !game::Date::isMonthChanged() )
    return;

  if( game::Date::current().year() > _d->lastDate.year() )
  {          
    _d->lastYearBalance = _city()->treasury().getIssueValue( econ::Issue::balance, econ::Treasury::lastYear );
    _d->workersSalary = _city()->treasury().workerSalary();

    _d->lastDate = game::Date::current();

    if( _city()->states().population == 0 )
    {
      _d->prosperity = 0;
      _d->prosperityExtend = 0;
      return;
    }

    HouseList houses = statistic::findh( _city() );

    int prosperityCap = 0;
    int patricianCount = 0;
    int plebsCount = 0;
    foreach( it, houses)
    {
      HousePtr house = *it;
      prosperityCap += house->spec().prosperity();
      patricianCount += house->spec().isPatrician() ? house->habitants().count() : 0;
      plebsCount += house->spec().level() < HouseLevel::plebsLevel ? house->habitants().count() : 0;
    }

    if( houses.size() > 0 )
    {
      prosperityCap /= houses.size();
    }

    _d->lastYearProsperity = value();

    int saveValue = _d->prosperity;
    _d->prosperity = math::clamp<int>( prosperityCap, 0, _d->prosperity + 2 );
    _d->houseCapTrand = _d->prosperity - saveValue;

    int currentFunds = _city()->treasury().money();
    _d->makeProfit = _d->lastYearBalance < currentFunds;
    _d->lastYearBalance = currentFunds;
    _d->prosperityExtend = (_d->makeProfit ? prosperity::cityHaveProfitAward : prosperity::penalty );

    bool more10PercentIsPatrician = math::percentage( patricianCount, _city()->states().population ) > 10;
    _d->prosperityExtend += (more10PercentIsPatrician ? prosperity::award : 0);

    _d->percentPlebs = math::percentage( plebsCount, _city()->states().population );
    _d->prosperityExtend += (_d->percentPlebs < prosperity::normalPlebsInCityPercent ? prosperity::award : 0);

    bool haveHippodrome = !statistic::findo<Hippodrome>( _city(), object::hippodrome ).empty();
    _d->prosperityExtend += (haveHippodrome ? prosperity::award : 0);

    _d->worklessPercent = statistic::getWorklessPercent( _city() );
    bool unemploymentLess5percent = _d->worklessPercent < prosperity::normalWorklesPercent;
    bool unemploymentMore15percent = _d->worklessPercent > workless::high;

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
  }
}

int ProsperityRating::value() const {  return _d->prosperity + _d->prosperityExtend; }

int ProsperityRating::getMark(ProsperityRating::Mark type) const
{
  switch( type )
  {
  case housesCap: return _d->houseCapTrand;
  case haveProfit: return _d->makeProfit;
  case worklessPercent: return _d->worklessPercent;
  case workersSalary: return _d->workersSalary;
  case changeValue: return value() - _d->lastYearProsperity;
  case plebsPercent: return _d->percentPlebs;
  }

  return 0;
}

std::string ProsperityRating::defaultName(){  return CAESARIA_STR_EXT(ProsperityRating); }

VariantMap ProsperityRating::save() const
{
  VariantMap ret;

  return ret;
}

void ProsperityRating::load(const VariantMap& stream)
{

}

}//end namespace city
