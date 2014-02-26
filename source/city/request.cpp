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

#include "city.hpp"
#include "request.hpp"
#include "good/goodhelper.hpp"
#include "statistic.hpp"
#include "events/removegoods.hpp"
#include "events/fundissue.hpp"
#include "city/funds.hpp"
#include "core/stringhelper.hpp"
#include "core/gettext.hpp"

class GoodRequest::Impl
{
public:
  DateTime date;
  unsigned int months2comply;
  GoodStock stock;
  int winFavour, winMoney;
  int failFavour, failMoney, failAppendMonth;
  std::string description;
};

CityRequestPtr GoodRequest::create( const VariantMap& stream )
{
  GoodRequest* gr = new GoodRequest();
  gr->load( stream );

  CityRequestPtr ret( gr );
  ret->drop();

  return ret;
}

GoodRequest::~GoodRequest(){}

void GoodRequest::exec( PlayerCityPtr city )
{
  if( !isDeleted() )
  {
    events::GameEventPtr e = events::RemoveGoods::create( _d->stock.type(), _d->stock.capacity() * 100 );
    e->dispatch();
    success( city );
  }
}

bool GoodRequest::mayExec( PlayerCityPtr city ) const
{
  CityStatistic::GoodsMap gm = CityStatistic::getGoodsMap( city );

  _d->description = StringHelper::format( 0xff, "%s %d", _("##qty_stacked_in_city_warehouse##"), gm[ _d->stock.type() ] / 100 );
  if( gm[ _d->stock.type() ] >= _d->stock.capacity() * 100 )
  {
    return true;
  }

  _d->description += std::string( "      " ) + _( "##unable_fullfill_request##" );
  return false;
}

std::string GoodRequest::typeName() {  return "good_request";}

VariantMap GoodRequest::save() const
{
  VariantMap ret = CityRequest::save();
  ret[ "date" ] = _d->date;
  ret[ "type" ] = Variant( typeName() );
  ret[ "month" ] = _d->months2comply;
  ret[ "good" ] = _d->stock.save();
  VariantMap vm_win;
  vm_win[ "favour" ] = _d->winFavour;
  vm_win[ "money" ] = _d->winMoney;
  ret[ "success" ] = vm_win;

  VariantMap vm_fail;
  vm_fail[ "favour" ] = _d->failFavour;
  vm_fail[ "money" ] = _d->failMoney;
  vm_fail[ "appendMonth" ] = _d->failAppendMonth;
  ret[ "fail" ] = vm_fail;

  return ret;
}

void GoodRequest::load(const VariantMap& stream)
{
  _d->date = stream.get( "date" ).toDateTime();
  _d->months2comply = (int)stream.get( "month" );

  Variant vm_goodt = stream.get( "good" );
  if( vm_goodt.type() == Variant::Map )
  {
    VariantMap vm_good = vm_goodt.toMap();
    if( !vm_good.empty() )
    {
      _d->stock.setType( GoodHelper::getType( vm_good.begin()->first ) );
      _d->stock.setCapacity( vm_good.begin()->second.toInt() );
    }
  }
  else if( vm_goodt.type() == Variant::List )
  {
    _d->stock.load( vm_goodt.toList() );
  }

  VariantMap vm_win = stream.get( "success" ).toMap();
  _d->winFavour = vm_win.get( "favour" );
  _d->winMoney = vm_win.get( "money" );

  VariantMap vm_fail = stream.get( "fail" ).toMap();
  _d->failFavour = vm_fail.get( "favour" );
  _d->failMoney = vm_fail.get( "money" );
  _d->failAppendMonth = vm_fail.get( "appendMonth" );
  _finishedDate = _d->date.appendMonth( _d->months2comply );
}

void GoodRequest::success( PlayerCityPtr city )
{
  CityRequest::success( city );
  city->updateFavour( _d->winFavour );
  if( _d->winMoney )
  {
    events::GameEventPtr e = events::FundIssueEvent::create( CityFunds::donation, _d->winMoney );
    e->dispatch();
  }
}

void GoodRequest::fail( PlayerCityPtr city )
{
  city->updateFavour( _d->winFavour );
  if( _d->failAppendMonth > 0 )
  {
    _d->date = _finishedDate;
    _finishedDate = _finishedDate.appendDay( _d->failAppendMonth );
    _d->failAppendMonth = 0;
  }
  else
  {
    CityRequest::fail( city );
  }
}

std::string GoodRequest::getDescription() const {  return _d->description; }
int GoodRequest::getQty() const { return _d->stock.capacity(); }
Good::Type GoodRequest::getGoodType() const { return _d->stock.type(); }
int GoodRequest::getMonths2Comply() const { return _d->months2comply; }
GoodRequest::GoodRequest() : CityRequest( DateTime() ), _d( new Impl ) {}
