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
#include "city/helper.hpp"
#include "core/safetycast.hpp"
#include "core/variant_map.hpp"
#include "core/position.hpp"
#include "city/statistic.hpp"
#include "objects/house.hpp"
#include "objects/house_level.hpp"
#include "gfx/tile.hpp"
#include "objects/entertainment.hpp"
#include "game/gamedate.hpp"
#include "city/funds.hpp"
#include "world/empire.hpp"
#include "objects/hippodrome.hpp"
#include "objects/constants.hpp"
#include "cityservice_info.hpp"

using namespace  constants;

namespace city
{

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
    _d->lastYearBalance = _city()->funds().getIssueValue( city::Funds::balance, city::Funds::lastYear );
    _d->workersSalary = _city()->funds().workerSalary();

    _d->lastDate = game::Date::current();

    if( _city()->population() == 0 )
    {
      _d->prosperity = 0;
      _d->prosperityExtend = 0;
      return;
    }

    Helper helper( _city() );
    HouseList houses = helper.find<House>( objects::house );

    int prosperityCap = 0;
    int patricianCount = 0;
    int plebsCount = 0;
    foreach( it, houses)
    {
      HousePtr house = *it;
      prosperityCap += house->spec().prosperity();
      patricianCount += house->spec().isPatrician() ? house->habitants().count() : 0;
      plebsCount += house->spec().level() < 5 ? house->habitants().count() : 0;
    }

    if( houses.size() > 0 )
    {
      prosperityCap /= houses.size();
    }

    _d->lastYearProsperity = value();

    int saveValue = _d->prosperity;
    _d->prosperity = math::clamp<int>( prosperityCap, 0, _d->prosperity + 2 );
    _d->houseCapTrand = _d->prosperity - saveValue;

    int currentFunds = _city()->funds().treasury();
    _d->makeProfit = _d->lastYearBalance < currentFunds;
    _d->lastYearBalance = currentFunds;
    _d->prosperityExtend = (_d->makeProfit ? 2 : -1);

    bool more10PercentIsPatrician = (patricianCount / (float)_city()->population()) > 0.1;
    _d->prosperityExtend += (more10PercentIsPatrician ? 1 : 0);

    _d->percentPlebs = math::percentage( plebsCount, _city()->population() );
    _d->prosperityExtend += (_d->percentPlebs < 30 ? 1 : 0);

    bool haveHippodrome = !helper.find<Hippodrome>( objects::hippodrome ).empty();
    _d->prosperityExtend += (haveHippodrome ? 1 : 0);

    _d->worklessPercent = statistic::getWorklessPercent( _city() );
    bool unemploymentLess5percent = _d->worklessPercent < 5;
    bool unemploymentMore15percent = _d->worklessPercent > 15;

    _d->prosperityExtend += (unemploymentLess5percent ? 1 : 0);
    _d->prosperityExtend += (unemploymentMore15percent ? -1 : 0);

    bool havePatrician = patricianCount > 0;
    _d->prosperityExtend += (havePatrician ? 1 : 0);

    _d->workersSalary = _city()->funds().workerSalary() - _city()->empire()->workerSalary();
    _d->prosperityExtend += (_d->workersSalary > 0 ? 1 : 0);
    _d->prosperityExtend += (_d->workersSalary < 0 ? -1 : 0);
   
    _d->prosperityExtend += (_city()->haveOverduePayment() ? -3 : 0);
    _d->prosperityExtend += (_city()->isPaysTaxes() ? -3 : 0);

    unsigned int caesarsHelper = _city()->funds().getIssueValue( city::Funds::caesarsHelp, city::Funds::thisYear );
    caesarsHelper += _city()->funds().getIssueValue( city::Funds::caesarsHelp, city::Funds::lastYear );
    if( caesarsHelper > 0 )
      _d->prosperityExtend += -10;
  }
}

int ProsperityRating::value() const {  return _d->prosperity + _d->prosperityExtend; }

int ProsperityRating::getMark(ProsperityRating::Mark type) const
{
  switch( type )
  {
  case cmHousesCap: return _d->houseCapTrand;
  case cmHaveProfit: return _d->makeProfit;
  case cmWorkless: return _d->worklessPercent;
  case cmWorkersSalary: return _d->workersSalary;
  case cmChange: return value() - _d->lastYearProsperity;
  case cmPercentPlebs: return _d->percentPlebs;
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
