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

#include "timer.hpp"
#include "city/cityservice_timers.hpp"

class Timer::Impl
{
public:
  unsigned int time;
  unsigned int startTime;
  int id;
  bool loop;
  bool isActive;

oc3_signals public:
  Signal1<int> onTimeoutASignal;
  Signal0<> onTimeoutSignal;
};

Timer::~Timer(void)
{
}

Timer::Timer() : _d( new Impl ) 
{
  _d->time = 0;
  _d->isActive = true;
  _d->loop = false;
  _d->startTime = 0;
}

TimerPtr Timer::create( unsigned int time, bool loop, int id/*=-1 */ )
{
  TimerPtr ret( new Timer() );
  ret->_d->time = time;
  ret->_d->loop = loop;
  ret->_d->id = id;
  ret->drop();

  city::Timers::getInstance().addTimer( ret );

  return ret;
}

void Timer::update( unsigned int time )
{
  if( !_d->startTime )
  {
    _d->startTime = time;
  }

  if( _d->isActive && ( time - _d->startTime > _d->time) )
  {
    _d->onTimeoutASignal.emit( _d->id );
    _d->onTimeoutSignal.emit();

    _d->isActive = false;

    if( _d->loop )
    {
      _d->startTime = time;
      _d->isActive = true;
    }
  }
}

void Timer::setTime( unsigned int time )
{
  _d->time = time;
}

void Timer::setLoop( bool loop )
{
  _d->loop = loop;
}

Signal1<int>& Timer::onTimeoutA()
{
  return _d->onTimeoutASignal;
}

Signal0<>& Timer::onTimeout()
{
  return _d->onTimeoutSignal;
}

bool Timer::isActive() const
{
  return _d->isActive;
}

void Timer::destroy()
{
  _d->isActive = false; 
}
