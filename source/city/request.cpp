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

#include "city.hpp"
#include "request.hpp"
#include "good/goodhelper.hpp"
#include "statistic.hpp"
#include "events/removegoods.hpp"
#include "events/fundissue.hpp"
#include "city/funds.hpp"
#include "core/stringhelper.hpp"
#include "core/gettext.hpp"
#include "events/showinfobox.hpp"
#include "events/updatefavour.hpp"
#include "game/gamedate.hpp"
#include "events/showrequestwindow.hpp"

namespace  city
{

namespace request
{

class RqGood::Impl
{
public:
  unsigned int months2comply;
  GoodStock stock;
  bool alsoRemind;
  int winFavour, winMoney;
  int failFavour, failMoney, failAppendMonth;
  std::string description;
};

RequestPtr RqGood::create( const VariantMap& stream )
{
  RqGood* gr = new RqGood();
  gr->load( stream );

  RequestPtr ret( gr );
  ret->drop();

  return ret;
}

RqGood::~RqGood(){}

void RqGood::exec( PlayerCityPtr city )
{
  if( !isDeleted() )
  {
    events::GameEventPtr e = events::RemoveGoods::create( _d->stock.type(), _d->stock.capacity() * 100 );
    e->dispatch();
    success( city );
  }
}

bool RqGood::isReady( PlayerCityPtr city ) const
{
  city::Statistic::GoodsMap gm = city::Statistic::getGoodsMap( city );

  _d->description = StringHelper::format( 0xff, "%s %d", _("##qty_stacked_in_city_warehouse##"), gm[ _d->stock.type() ] / 100 );
  if( gm[ _d->stock.type() ] >= _d->stock.capacity() * 100 )
  {
    return true;
  }

  if( !_d->alsoRemind && (_startDate.monthsTo( GameDate::current() ) > 12) )
  {
    const_cast<RqGood*>( this )->_d->alsoRemind = true;

    events::GameEventPtr e = events::ShowRequestInfo::create( const_cast<RqGood*>( this ), true );
    e->dispatch();
  }

  _d->description += std::string( "      " ) + _( "##unable_fullfill_request##" );
  return false;
}

std::string RqGood::typeName() {  return "good_request";}

VariantMap RqGood::save() const
{
  VariantMap ret = Request::save();

  ret[ "reqtype" ] = Variant( typeName() );
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

void RqGood::load(const VariantMap& stream)
{
  Request::load( stream );
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

  Variant v_finish = stream.get( "finish" );
  if( v_finish.isNull() )
  {
    _finishDate = _startDate;
    _finishDate.appendMonth( _d->months2comply );
  }
}

void RqGood::success( PlayerCityPtr city )
{
  Request::success( city );
  if( _d->winMoney > 0 )
  {
    events::GameEventPtr e = events::FundIssueEvent::create( city::Funds::donation, _d->winMoney );
    e->dispatch();
  }

  if( _d->winFavour > 0 )
  {
    events::GameEventPtr e = events::UpdateFavour::create( city->getName(), _d->winFavour );
    e->dispatch();
  }
}

void RqGood::fail( PlayerCityPtr city )
{
  events::GameEventPtr e = events::UpdateFavour::create( city->getName(), _d->failFavour );
  e->dispatch();

  if( _d->failAppendMonth > 0 )
  {
    _startDate = _finishDate;

    std::string text = StringHelper::format( 0xff, "You also have %d month to comply failed request", _d->failAppendMonth );
    e = events::ShowInfobox::create( "##request_failed##", text );
    e->dispatch();

    _finishDate.appendMonth( _d->failAppendMonth );
    _d->failAppendMonth = 0;
    setAnnounced( false );
  }
  else
  {
    Request::fail( city );

    std::string text = StringHelper::format( 0xff, "You failed request" );
    e = events::ShowInfobox::create( "##request_failed##", text );
    e->dispatch();
  }
}

std::string RqGood::getDescription() const {  return _d->description; }
int RqGood::getQty() const { return _d->stock.capacity(); }
Good::Type RqGood::getGoodType() const { return _d->stock.type(); }

RqGood::RqGood() : Request( DateTime() ), _d( new Impl )
{
  _d->alsoRemind = false;
}

VariantMap Request::save() const
{
  VariantMap ret;
  ret[ "deleted" ] = _isDeleted;
  ret[ "announced" ] = _isAnnounced;
  ret[ "finish" ] = _finishDate;
  ret[ "start" ] = _startDate;

  return ret;
}

void Request::load(const VariantMap& stream)
{
  _isDeleted = stream.get( "deleted" );
  _isAnnounced = stream.get( "announced" );
  _finishDate = stream.get( "finish" ).toDateTime();
  _startDate = stream.get( "date" ).toDateTime();
}

Request::Request(DateTime finish) : _isDeleted( false ), _isAnnounced( false ), _finishDate( finish )
{

}

}//end namespace request

}//end namespace city
