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
#include "dispatcher.hpp"
#include "objects/construction.hpp"
#include "core/variant_map.hpp"
#include "city/requestdispatcher.hpp"
#include "core/logger.hpp"
#include "city/cityservice_factory.hpp"
#include "factory.hpp"
#include "city/statistic.hpp"
#include "city/states.hpp"

namespace events
{

namespace {
static const unsigned int defaultCheckInterval = 50;
}

struct EventDelay
{
  int maxYear = 0;
  int year = 0;
  int month = 0;
  int day = 0;

  bool valid() const { return (year + month + day != 0); }

  void load( const VariantMap& vm )
  {
    VARIANT_LOAD_ANY(maxYear, vm)
    VARIANT_LOAD_ANY(year, vm)
    VARIANT_LOAD_ANY(month, vm)
    VARIANT_LOAD_ANY(day, vm)
  }

  VariantMap save() const
  {
    VariantMap ret;
    VARIANT_SAVE_ANY(ret, maxYear)
    VARIANT_SAVE_ANY(ret, year)
    VARIANT_SAVE_ANY(ret, month)
    VARIANT_SAVE_ANY(ret, day)

    return ret;
  }
};

class PostponeEvent::Impl
{
public:
  DateTime date;
  EventDelay delay;
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
  auto* event = new PostponeEvent();

  std::string::size_type reshPos = type.find( "#" );
  event->_name = type;
  event->_type = reshPos != std::string::npos
                      ? type.substr( reshPos+1 )
                      : type;

  event->load( stream );
  Logger::warningIf( event->_type.empty(), "PostponeEvent: unknown postpone event" );

  GameEventPtr ret( event );
  ret->drop();

  if( event->_type.empty() )
    return GameEventPtr();

  Logger::warning( "PostponeEvent: load event " + event->_name );
  return ret;
}

PostponeEvent::~PostponeEvent(){}

void PostponeEvent::_executeIncludeEvents()
{
  VariantMap actions = _d->options.get( "exec" ).toMap();
  if( actions.empty() )
    Dispatcher::instance().load( actions );
}

void PostponeEvent::_exec(Game& game, unsigned int)
{
  Logger::warning( "Start event name=" + _name + " type=" + _type );

  std::vector<Impl::Worker> actions{ makeDelegate( _d.data(), &Impl::executeRequest ),
                                     makeDelegate( _d.data(), &Impl::executeEvent ),
                                     makeDelegate( _d.data(), &Impl::executeCityService ) };

  for( auto& action : actions )
  {
    bool execOk;
    action( game, _type, execOk );
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
  if( _d->checkInterval == 0 )
  {
    bool dateCondition = true;

    if( _d->date.year() == -1000 )
    {
      if( _d->delay.valid() )
      {
        DateTime startDate = game.city()->states().birth;
        int addictiveYears = _d->delay.year == -1
                                  ? math::random( _d->delay.maxYear )
                                  : _d->delay.year;
        startDate = startDate.appendMonth( addictiveYears * DateTime::monthsInYear );

        int addictiveMonths = _d->delay.maxYear == -1
                                  ? math::random( DateTime::monthsInYear )
                                  : _d->delay.month;
        startDate = startDate.appendMonth( addictiveMonths );

        int addictiveDays = _d->delay.day == -1
                                  ? math::random( DateTime::daysInMonth( addictiveYears, addictiveMonths ) )
                                  : _d->delay.day;
        _d->date = startDate.appendDay( addictiveDays );
      }
    }

    if( _d->date.year() != -1000 )
    {
      dateCondition = _d->date <= game::Date::current();
    }

    bool popCondition = true;
    if( _d->population > 0 )
    {
      popCondition = game.city()->states().population > _d->population;
    }

    _d->mayDelete = dateCondition && popCondition;
    return _d->mayDelete;
  }

  if( _d->checkInterval > 0 )
  {
    _d->checkInterval--;
  }

  return false;
}

bool PostponeEvent::isDeleted() const{ return _d->mayDelete; }
VariantMap PostponeEvent::save() const
{
  VariantMap ret = _d->options;
  ret[ "type" ] = Variant( _type );
  ret[ "name" ] = Variant( _name );
  VARIANT_SAVE_ANY_D( ret, _d, date )
  VARIANT_SAVE_ANY_D( ret, _d, checkInterval)
  VARIANT_SAVE_ANY_D( ret, _d, population )
  if( _d->delay.valid() ) VARIANT_SAVE_CLASS_D( ret, _d, delay )
  return ret;
}

void PostponeEvent::load(const VariantMap& stream)
{  
  GameEvent::load( stream );

  _d->date = stream.get( "date", DateTime( -1000, 1, 1 ) ).toDateTime();
  VARIANT_LOAD_ANY_D( _d, population, stream )
  VARIANT_LOAD_CLASS_D( _d, delay, stream )
  VARIANT_LOAD_ANYDEF_D( _d, checkInterval, defaultCheckInterval, stream )

  _d->options = stream;
}

PostponeEvent::PostponeEvent() : _d( new Impl )
{
  _d->mayDelete = false;
  _d->checkInterval = 0;
}

void PostponeEvent::Impl::executeRequest( Game& game, const std::string& type, bool& r )
{
  if( "city_request" == type )
  {
    auto dispatcher = game.city()->statistic().services.find<city::request::Dispatcher>();

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

}//end namespace events
