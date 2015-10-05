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
// Copyright 2012-2013 Gregoire Athanase, gathanase@gmail.com
// Copyright 2012-2014 Dalerank, dalerankn8@gmail.com

#include "cityservice_timers.hpp"
#include "core/time.hpp"
#include <vector>

namespace city
{

typedef std::vector< TimerPtr > TimerList;

class Timers::Impl
{
public:
  TimerList timers;
};

Timers::Timers() : _d( new Impl )
{ 
}

void Timers::update( const unsigned int time )
{
  auto timerIt=_d->timers.begin();
  while( timerIt != _d->timers.end() )
  { 
    if( !(*timerIt)->isActive() )
    {
      timerIt = _d->timers.erase( timerIt );
    }
    else
    {
      (*timerIt)->update( time );
      ++timerIt;
    }
  }
}

void Timers::addTimer( TimerPtr timer ) {  _d->timers.push_back( timer ); }
void Timers::reset() { _d->timers.clear(); }

TimerPtr Timers::find(int id) const
{
  for( auto&& timer : _d->timers )
    if( timer->id() == id )
      return timer;

  return TimerPtr();
}

Timers::~Timers() {}

}//end namespace city
