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

#include "requestdispatcher.hpp"
#include "request.hpp"
#include "city/city.hpp"
#include "game/gamedate.hpp"
#include "events/showrequestwindow.hpp"
#include "core/variant_map.hpp"
#include "core/foreach.hpp"
#include "core/logger.hpp"
#include "core/utils.hpp"
#include "cityservice_factory.hpp"

namespace city
{

namespace request
{

REGISTER_SERVICE_IN_FACTORY(Dispatcher,requestDispatcher)

class Dispatcher::Impl
{
public:
  RequestList requests;
  RequestList newRequests;
  DateTime lastRequestCancelDate;

public:
  void updateRequests();
  void weekUpdate( unsigned int time, PlayerCityPtr city );
};

Dispatcher::Dispatcher( PlayerCityPtr city )
  : Srvc( city, defaultName() ), _d( new Impl )
{
}

city::SrvcPtr Dispatcher::create( PlayerCityPtr city )
{
  SrvcPtr ret( new Dispatcher( city ) );
  ret->drop();

  return ret;
}

bool Dispatcher::add( const VariantMap& stream, bool showMessage )
{
  const std::string type = stream.get( "reqtype" ).toString();
  if( type == RqGood::typeName() )
  {
    RequestPtr r = RqGood::create( stream );
    _d->newRequests.push_back( r );

    if( showMessage )
    {
      events::GameEventPtr e = events::ShowRequestInfo::create( r );
      e->dispatch();
    }
    return true;
  }

  Logger::warning( "WARNING!!! CityRequestDispatcher: cannot load request with type " + type );
  return false;
}

Dispatcher::~Dispatcher() {}
std::string Dispatcher::defaultName(){  return "requests";}

void Dispatcher::Impl::weekUpdate( unsigned int time, PlayerCityPtr rcity )
{
  const DateTime current = game::Date::current();
  foreach( rq, requests )
  {
    RequestPtr request = *rq;
    if( request->finishedDate() <= current )
    {
      request->fail( rcity );
      lastRequestCancelDate = current;
    }

    bool isReady = request->isReady( rcity );
    if( !request->isAnnounced() && isReady )
    {
      events::GameEventPtr e = events::ShowRequestInfo::create( request, true );
      request->setAnnounced( true );
      e->dispatch();
    }

    request->update();
  }
}

void Dispatcher::timeStep(const unsigned int time)
{
  if( game::Date::isWeekChanged() )
  {
    _d->weekUpdate( time, _city() );
  }

  if( game::Date::isDayChanged() )
  {
    _d->updateRequests();
  }
}

VariantMap Dispatcher::save() const
{
  VariantMap ret;
  VariantMap vm_rq;

  std::string name;
  name.reserve( 256 );
  foreach( rq, _d->requests )
  {
    name = utils::format( 0xff, "request_%02d", std::distance( _d->requests.begin(), rq ) );
    vm_rq[ name ] = (*rq)->save();
  }

  ret[ "items" ] = vm_rq;
  VARIANT_SAVE_ANY_D( ret, _d, lastRequestCancelDate )
  return ret;
}

void Dispatcher::load(const VariantMap& stream)
{
  VariantMap vm_items = stream.get( "items" ).toMap();
  foreach( it, vm_items )
  {
    add( it->second.toMap(), false );
  }

  VARIANT_LOAD_TIME_D( _d, lastRequestCancelDate, stream )
}

bool Dispatcher::haveCanceledRequest() const
{
  return _d->lastRequestCancelDate.monthsTo( game::Date::current() ) < DateTime::monthsInYear;
}

RequestList Dispatcher::requests() const {  return _d->requests; }

void Dispatcher::Impl::updateRequests()
{
  utils::eraseDeletedElements( requests );

  if( !newRequests.empty() )
  {
    requests << newRequests;
    newRequests.clear();
  }
}

}//end namespace request

}//end namespace city
