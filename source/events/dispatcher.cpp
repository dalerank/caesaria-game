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

#include "dispatcher.hpp"
#include "core/stringhelper.hpp"
#include "core/foreach.hpp"
#include "loader.hpp"

namespace events
{

class Dispatcher::Impl
{
public:
  typedef std::list< GameEventPtr > Events;

  Events events;

public oc3_signals:
  Signal1<GameEventPtr> onEventSignal;
};

Dispatcher::Dispatcher() : _d( new Impl )
{
}

Dispatcher::~Dispatcher()
{

}

void Dispatcher::append( GameEventPtr event)
{
  _d->events.push_back( event );
}

void Dispatcher::update(unsigned int time)
{
  for( Impl::Events::iterator it=_d->events.begin(); it != _d->events.end();  )
  {
    GameEventPtr e = *it;
    if( e->mayExec( time ) )
    {
      _d->onEventSignal.emit( e );
    }

    if( e->isDeleted() ) { it = _d->events.erase( it ); }
    else { it++; }
  }
}

VariantMap Dispatcher::save() const
{
  VariantMap ret;
  int index = 0;
  foreach( GameEventPtr event, _d->events )
  {
    ret[ StringHelper::format( 0xff, "%d", index ) ] = event->save();
  }

  return ret;
}

void Dispatcher::load(const VariantMap& stream)
{
  for( VariantMap::const_iterator it=stream.begin();
       it != stream.end(); it++ )
  {
    GameEventPtr e = Loader::load( it->second.toMap() );

    if( e.isValid() )
    {
      append( e );
    }
  }
}

Signal1<GameEventPtr>&Dispatcher::onEvent()
{
  return _d->onEventSignal;
}

}//end namespace events
