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

#include "dispatcher.hpp"
#include "core/utils.hpp"
#include "core/foreach.hpp"
#include "postpone.hpp"
#include "core/variant_map.hpp"
#include "core/logger.hpp"
#include "core/stacktrace.hpp"
#include "core/saveadapter.hpp"

namespace events
{

class Dispatcher::Impl
{
public:
  typedef SmartList< GameEvent > Events;

  Events events;
  Events newEvents;
};

Dispatcher::Dispatcher() : _d( new Impl )
{
}

Dispatcher::~Dispatcher(){}

void Dispatcher::append(GameEventPtr event)
{
  if( event.isValid() )
  {
    _d->newEvents.push_back( event );
  }
  else
  {
    Logger::warning( "EventsDispatcher: cant add event but is null" );
    Stacktrace::print();
  }
}

void Dispatcher::update(Game& game, unsigned int time )
{
  for( Impl::Events::iterator it=_d->events.begin(); it != _d->events.end();  )
  {
    GameEventPtr e = *it;

    try
    {
      e->tryExec( game, time );

      if( e->isDeleted() ) { it = _d->events.erase( it ); }
      else { ++it; }
    }
    catch(...)
    {
      _d->events.erase( it );
    }
  }

  if( !_d->newEvents.empty() )
  {
    _d->events << _d->newEvents;
    _d->newEvents.clear();
  }
}

VariantMap Dispatcher::save() const
{
  VariantMap ret;
  int index = 0;
  foreach( event, _d->events )
  {
    ret[ utils::format( 0xff, "event_%d", index++ ) ] = (*event)->save();
  }

  return ret;
}

void Dispatcher::load(const VariantMap& stream)
{
  foreach( it, stream )
  {
    GameEventPtr e = PostponeEvent::create( it->first, it->second.toMap() );

    if( e.isValid() )
    {
      append( e );
    }
  }
}

void Dispatcher::load(vfs::Path filename, const std::string& section)
{
  VariantMap vm = config::load( filename );

  if( !section.empty() )
    vm = vm.get( section ).toMap();

  load( vm );
}

void Dispatcher::reset() { _d->events.clear(); }

}//end namespace events
