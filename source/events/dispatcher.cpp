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
#include "core/stringhelper.hpp"
#include "core/foreach.hpp"
#include "postpone.hpp"
#include "core/logger.hpp"

namespace events
{

class Dispatcher::Impl
{
public:
  typedef SmartList< GameEvent > Events;

  Events events;
};

Dispatcher::Dispatcher() : _d( new Impl )
{
}

Dispatcher::~Dispatcher(){}

void Dispatcher::append(GameEventPtr event)
{
  if( event.isValid() )
  {
    _d->events.push_back( event );
  }
  else
  {
    Logger::warning( "Null event" );
  }
}

void Dispatcher::update(Game& game, unsigned int time )
{
  for( Impl::Events::iterator it=_d->events.begin(); it != _d->events.end();  )
  {
    GameEventPtr e = *it;

    e->tryExec( game, time );

    if( e->isDeleted() ) { it = _d->events.erase( it ); }
    else { ++it; }
  }
}

VariantMap Dispatcher::save() const
{
  VariantMap ret;
  int index = 0;
  foreach( event, _d->events )
  {
    ret[ StringHelper::format( 0xff, "event_%d", index++ ) ] = (*event)->save();
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

void Dispatcher::reset() { _d->events.clear(); }

}//end namespace events
