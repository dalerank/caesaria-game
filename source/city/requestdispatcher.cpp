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

#include "city.hpp"
#include "requestdispatcher.hpp"
#include "request.hpp"
#include "game/gamedate.hpp"
#include "events/showrequestwindow.hpp"
#include "core/variant_map.hpp"
#include "core/foreach.hpp"
#include "core/logger.hpp"
#include "core/utils.hpp"

namespace city
{

namespace request
{

class Dispatcher::Impl
{
public:
  RequestList requests;
  RequestList newRequests;
  DateTime lastRequestCancelDate;

public:
  void updateRequests();
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

void Dispatcher::timeStep(const unsigned int time)
{
  if( game::Date::isWeekChanged() )
  {
    foreach( rq, _d->requests )
    {
      RequestPtr request = *rq;
      if( request->finishedDate() <= game::Date::current() )
      {
        request->fail( _city() );
        _d->lastRequestCancelDate = game::Date::current();
      }

      bool isReady = request->isReady( _city() );
      isReady;

      if( !request->isAnnounced() )
      {
        events::GameEventPtr e = events::ShowRequestInfo::create( request, true );
        request->setAnnounced( true );
        e->dispatch();
      }

      request->update();
    }

    _d->updateRequests();
  }
}

VariantMap Dispatcher::save() const
{
  VariantMap ret;
  VariantMap vm_rq;

  foreach( rq, _d->requests )
  {
    std::string name = utils::format( 0xff, "request_%02d", std::distance( _d->requests.begin(), rq ) );
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
  for( RequestList::iterator i=requests.begin(); i != requests.end(); )
  {
    if( (*i)->isDeleted() ) { i = requests.erase( i ); }
    else { ++i; }
  }

  requests << newRequests;
  newRequests.clear();
}

}//end namespace request

}//end namespace city
