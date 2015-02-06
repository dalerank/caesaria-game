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

#include "postpone.hpp"
#include "game/gamedate.hpp"
#include "game/game.hpp"
#include "city/city.hpp"
#include "core/foreach.hpp"
#include "dispatcher.hpp"
#include "core/variant_map.hpp"
#include "city/requestdispatcher.hpp"
#include "core/logger.hpp"
#include "city/cityservice_factory.hpp"
#include "factory.hpp"

namespace events
{

namespace {
static const unsigned int defaultCheckInterval = 50;
}

class PostponeEvent::Impl
{
public:
  DateTime date;
  unsigned int population;
  bool mayDelete;
  unsigned int checkInterval;
  VariantMap options;

  void executeRequest( Game& game, const std::string& type, bool& result );
  void executeEvent( Game& game, const std::string& type, bool& result );
  void executeCityService( Game& game, const std::string& type, bool& result );

  typedef Delegate3< Game&, const std::string&, bool& > Worker;
};

GameEventPtr PostponeEvent::create( const std::string& type, const VariantMap& stream )
{
  PostponeEvent* pe = new PostponeEvent();

  std::string::size_type reshPos = type.find( "#" );
  pe->_name = type;
  pe->_type = reshPos != std::string::npos
                      ? type.substr( reshPos+1 )
                      : type;

  pe->load( stream );
  Logger::warningIf( pe->_type.empty(), "PostponeEvent: unknown postpone event" );

  GameEventPtr ret( pe );
  ret->drop();

  if( pe->_type.empty() )
    return GameEventPtr();

  Logger::warning( "PostponeEvent: load event " + pe->_name );
  return ret;
}

PostponeEvent::~PostponeEvent(){}

void PostponeEvent::_executeIncludeEvents()
{
  Variant execVm = _d->options.get( "exec" );
  if( execVm.isValid() )
  {
    Dispatcher::instance().load( execVm.toMap() );
  }
}

void PostponeEvent::_exec(Game& game, unsigned int)
{
  Logger::warning( "Start event name=" + _name + " type=" + _type );

  Impl::Worker workers[3] = { makeDelegate( _d.data(), &Impl::executeRequest ),
                              makeDelegate( _d.data(), &Impl::executeEvent ),
                              makeDelegate( _d.data(), &Impl::executeCityService ) };

  for( int i=0; i < 3; i++ )
  {
    bool execOk;
    workers[i]( game, _type, execOk );
    if( execOk )
    {
      _executeIncludeEvents();
      return;
    }
  }

  _d->mayDelete = true;
}

bool PostponeEvent::_mayExec( Game& game, unsigned int time ) const
{
  if( _d->checkInterval == 0 || (time % _d->checkInterval == 1) )
  {
    bool dateCondition = true;
    if( _d->date.year() != -1000 )
    {
      dateCondition = _d->date <= game::Date::current();
    }

    bool popCondition = true;
    if( _d->population > 0 )
    {
      popCondition = game.city()->population() > _d->population;
    }

    _d->mayDelete = dateCondition && popCondition;
    return _d->mayDelete;
  }

  return false;
}

bool PostponeEvent::isDeleted() const{  return _d->mayDelete; }
VariantMap PostponeEvent::save() const
{
  VariantMap ret = _d->options;
  ret[ "type" ] = Variant( _type );
  ret[ "name" ] = Variant( _name );  
  ret[ "date" ] = _d->date;
  VARIANT_SAVE_ANY_D( ret, _d, checkInterval)
  VARIANT_SAVE_ANY_D( ret, _d, population )
  return ret;
}

void PostponeEvent::load(const VariantMap& stream)
{  
  GameEvent::load( stream );

  _d->date = stream.get( "date", DateTime( -1000, 1, 1 ) ).toDateTime();
  VARIANT_LOAD_ANY_D( _d, population, stream )
  VARIANT_LOAD_ANYDEF_D( _d, checkInterval, defaultCheckInterval, stream )
  _d->options = stream;
}

PostponeEvent::PostponeEvent() : _d( new Impl )
{
  _d->mayDelete = false;
}

void PostponeEvent::Impl::executeRequest( Game& game, const std::string& type, bool& r )
{
  if( "city_request" == type )
  {
    PlayerCityPtr city = game.city();

    city::SrvcPtr service = city->findService( city::request::Dispatcher::defaultName() );
    city::request::DispatcherPtr dispatcher = ptr_cast<city::request::Dispatcher>( service );

    if( dispatcher.isValid() )
    {
      dispatcher->add( options );
    }
    r = true;
    return;
  }

  r = false;
}

void PostponeEvent::Impl::executeEvent( Game& game, const std::string& type, bool& r  )
{
  GameEventPtr e = EFactory::create( type );
  if( e.isValid() )
  {
    e->load( options );
    e->dispatch();
    r = true;
    return;
  }

  r = false;
}

void PostponeEvent::Impl::executeCityService( Game& game, const std::string& type, bool& r )
{
  PlayerCityPtr city = game.city();
  std::string dtype = options.get( "type", Variant( type ) ).toString();
  city::SrvcPtr srvc = city::ServiceFactory::create( game.city(), dtype );
  if( srvc.isValid() )
  {
    srvc->load( options );
    city->addService( srvc );
    r = true;
    return;
  }

  r = false;
}

}
