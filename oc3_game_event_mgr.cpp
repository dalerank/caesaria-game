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

#include "oc3_game_event_mgr.hpp"
#include "oc3_foreach.hpp"
#include "oc3_city.hpp"
#include <vector>

class GameEventMgr::Impl
{
public:
  typedef std::vector< GameEventPtr > Events;

  Events events;

public oc3_signals:
  Signal1<GameEventPtr> onEventSignal;
};

GameEventMgr::GameEventMgr() : _d( new Impl )
{
}

GameEventMgr::~GameEventMgr()
{

}

void GameEventMgr::append( GameEventPtr event)
{
  instance()._d->events.push_back( event );
}

void GameEventMgr::update(unsigned int time)
{
  GameEventMgr& inst = instance();
  Impl::Events events = inst._d->events;
  inst._d->events.clear();

  foreach( GameEventPtr event, events )
  {
   inst._d->onEventSignal.emit( event );
  }
}

Signal1<GameEventPtr>&GameEventMgr::onEvent()
{
  return _d->onEventSignal;
}
