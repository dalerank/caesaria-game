// This file is part of openCaesar3.
//
// openCaesar3 is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// openCaesar3 is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with openCaesar3.  If not, see <http://www.gnu.org/licenses/>.

#include "dispatcher.hpp"
#include "core/foreach.hpp"
#include <vector>

namespace events
{

class Dispatcher::Impl
{
public:
  typedef std::vector< GameEventPtr > Events;

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
  instance()._d->events.push_back( event );
}

void Dispatcher::update(unsigned int time)
{
  Dispatcher& inst = instance();
  Impl::Events events = inst._d->events;
  inst._d->events.clear();

  foreach( GameEventPtr event, events )
  {
   inst._d->onEventSignal.emit( event );
  }
}

Signal1<GameEventPtr>&Dispatcher::onEvent()
{
  return _d->onEventSignal;
}

}//end namespace events
