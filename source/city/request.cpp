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
#include "good/helper.hpp"
#include "statistic.hpp"
#include "events/removegoods.hpp"
#include "objects/construction.hpp"
#include "events/fundissue.hpp"
#include "game/funds.hpp"
#include "world/relations.hpp"
#include "core/utils.hpp"
#include "core/gettext.hpp"
#include "events/showinfobox.hpp"
#include "events/updatefavour.hpp"
#include "game/gamedate.hpp"
#include "world/empire.hpp"
#include "world/emperor.hpp"
#include "events/showrequestwindow.hpp"
#include "world/goodcaravan.hpp"
#include "good/store.hpp"
#include "core/metric.hpp"
#include "core/logger.hpp"
#include "core/variant_map.hpp"

using namespace metric;
using namespace events;

namespace city
{

namespace request
{

struct Award
{
  int favour;
  int money;

  VariantMap save() const
  {
    VariantMap vm_win;
    VARIANT_SAVE_ANY( vm_win, favour )
    VARIANT_SAVE_ANY( vm_win, money )

    return vm_win;
  }

  void load( const VariantMap& vm )
  {
    VARIANT_LOAD_ANY( favour, vm )
    VARIANT_LOAD_ANY( money, vm )
  }

  void apply( PlayerCityPtr city ) const
  {
    if( money > 0 )
    {
      events::dispatch<Payment>( econ::Issue::donation, money );
    }

    if( favour > 0 )
    {
      events::dispatch<UpdateFavour>( city->name(), favour );
    }
  }
};

struct Penalty
{
  int favour;
  int money;
  int appendMonth;

  VariantMap save() const
  {
    VariantMap vm_fail;
    VARIANT_SAVE_ANY( vm_fail, favour )
    VARIANT_SAVE_ANY( vm_fail, money )
    VARIANT_SAVE_ANY( vm_fail, appendMonth )

    return vm_fail;
  }

  void load( const VariantMap& vm )
  {
    VARIANT_LOAD_ANY( favour, vm )
    VARIANT_LOAD_ANY( money, vm )
    VARIANT_LOAD_ANY( appendMonth, vm )
  }

  void apply( PlayerCityPtr city ) const
  {
    events::dispatch<UpdateFavour>( city->name(), favour );
  }
};

class RqGood::Impl
{
public:
  unsigned int months2comply;
  good::Stock stock;
  bool alsoRemind;
  Award complyRequest;
  Penalty failedRequest;
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
    Unit stockCap = Unit::fromValue( _d->stock.capacity() );
    good::Stock stock( _d->stock.type(), stockCap.toQty() );

    events::dispatch<RemoveGoods>( stock.type(), stock.capacity() );
    success( city );

    auto caravan = world::GoodCaravan::create( ptr_cast<world::City>( city ) );
    caravan->store().store( stock, -1 );
    caravan->sendTo( city->empire()->capital() );
  }
}

bool RqGood::isReady( PlayerCityPtr city ) const
{
  good::ProductMap gm = city->statistic().goods.details( false );

  Unit stockCap = Unit::fromQty( gm[ _d->stock.type() ] );
  Unit needCap = Unit::fromValue( _d->stock.capacity() );
  _d->description = fmt::format( "{0} {1}", _("##qty_stacked_in_city_warehouse##"), stockCap.ivalue() );
  if( stockCap >= needCap )
  {
    return true;
  }

  _d->description += std::string( "      " ) + _( "##unable_fullfill_request##" );
  return false;
}

std::string RqGood::typeName() {  return "good_request";}

VariantMap RqGood::save() const
{
  VariantMap ret = RqBase::save();

  ret[ "reqtype" ] = Variant( typeName() );
  ret[ "month" ] = _d->months2comply;
  ret[ "good" ] = _d->stock.save();
  VARIANT_SAVE_ANY_D( ret, _d, alsoRemind )

  ret[ "success" ] = _d->complyRequest.save();
  ret[ "fail" ] = _d->failedRequest.save();

  return ret;
}

void RqGood::load(const VariantMap& stream)
{
  RqBase::load( stream );
  _d->months2comply = (int)stream.get( "month" );

  Variant vm_goodt = stream.get( "good" );
  VARIANT_LOAD_ANY_D( _d, alsoRemind, stream )
  if( vm_goodt.type() == Variant::Map )
  {
    VariantMap vm_good = vm_goodt.toMap();
    if( !vm_good.empty() )
    {
      _d->stock.setType( good::Helper::getType( vm_good.begin()->first ) );
      _d->stock.setCapacity( vm_good.begin()->second.toInt() );
    }
  }
  else if( vm_goodt.type() == Variant::List )
  {
    _d->stock.load( vm_goodt.toList() );
  }

  _d->complyRequest.load( stream.get( "success" ).toMap() );
  _d->failedRequest.load( stream.get( "fail" ).toMap() );

  Variant v_finish = stream.get( "finish" );
  if( v_finish.isNull() )
  {
    _finishDate = _startDate;
    _finishDate.appendMonth( _d->months2comply );
  }
}

void RqGood::success( PlayerCityPtr city )
{
  RqBase::success( city );
  _d->complyRequest.apply( city );  
}

void RqGood::fail( PlayerCityPtr city )
{
  _d->failedRequest.apply( city );

  if( _d->failedRequest.appendMonth > 0 )
  {
    _startDate = _finishDate;

    //std::string text = utils::format( 0xff, "You also have %d month to comply failed request", _d->failAppendMonth );
    events::dispatch<ShowInfobox>( _("##emperor_anger##"), _("##emperor_anger_text##") );

    _finishDate.appendMonth( _d->failedRequest.appendMonth );
    _d->failedRequest.appendMonth = 0;
    setAnnounced( false );
  }
  else
  {
    RqBase::fail( city );

    events::dispatch<ShowInfobox>( _("##emperor_anger##"), _("##request_faild_text##") );
  }
}

void RqGood::update()
{
  RqBase::update();

  if( !_d->alsoRemind && (_startDate.monthsTo( game::Date::current() ) > DateTime::monthsInYear ) )
  {
    _d->alsoRemind = true;

    events::dispatch<ShowRequestInfo>( this, true, _("##imperial_reminder_text##"), "", _("##imperial_reminder##") );
  }
}

std::string RqGood::description() const {  return _d->description; }
int RqGood::qty() const { return _d->stock.capacity(); }
good::Product RqGood::goodType() const { return _d->stock.type(); }

RqGood::RqGood() : RqBase( DateTime() ), _d( new Impl )
{
  _d->alsoRemind = false;
}

VariantMap RqBase::save() const
{
  VariantMap ret;
  ret[ "deleted" ] = _isDeleted;
  ret[ "announced" ] = _isAnnounced;
  ret[ "finish" ] = _finishDate;
  ret[ "date" ] = _startDate;

  return ret;
}

void RqBase::load(const VariantMap& stream)
{
  _isDeleted = stream.get( "deleted" );
  _isAnnounced = stream.get( "announced" );
  _finishDate = stream.get( "finish" ).toDateTime();

  Variant vStart = stream.get( "date" );
  Logger::warningIf( vStart.isNull(), "Request: unknown start date" );

  _startDate = vStart.isNull() ? game::Date::current() : vStart.toDateTime();
}

void RqBase::_saveState(PlayerCityPtr city, int relation, const std::string& message)
{
  world::Emperor& emp = city->empire()->emperor();
  world::RelationAbility ability;

  ability.start = startDate();
  ability.finished = finishedDate();
  ability.successed = isSuccessed();
  ability.relation = relation;
  ability.message = message;
  ability.type = world::RelationAbility::request;
  ability.influenceMonth = 0;

  emp.updateRelation( city->name(), ability );
}

RqBase::RqBase(DateTime finish) :
  _isDeleted( false ), _isAnnounced( false ),
  _isSuccessed( false ),
  _finishDate( finish )
{

}

}//end namespace request

}//end namespace city
