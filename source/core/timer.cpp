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

#include "timer.hpp"
#include "city/cityservice_timers.hpp"
#include "core/utils.hpp"
#include "core/logger.hpp"
#include <SDL_cpuinfo.h>
#include <SDL_timer.h>

class Timer::Impl
{
public:
  struct {
  unsigned int interval;
  unsigned int start;
  } time;

  int id;
  bool loop;
  bool isActive;

  struct {
    Signal1<int> onTimeoutA;
    Signal0<> onTimeout;
  } signal;
};

Timer::~Timer(void) {}

Timer::Timer() : _d( new Impl ) 
{
  _d->time.interval = 0;
  _d->isActive = true;
  _d->loop = false;
  _d->time.start = 0;
}

TimerPtr Timer::create( unsigned int time, bool loop, int id/*=-1 */ )
{
  TimerPtr ret( new Timer() );
  ret->_d->time.interval = time;
  ret->_d->loop = loop;
  ret->_d->id = id;
  ret->drop();

  city::Timers::instance().addTimer( ret );

  return ret;
}

void Timer::destroy(int id)
{
  TimerPtr timer = city::Timers::instance().find( id );
  if( timer.isValid() )
    timer->destroy();
}

void Timer::update( unsigned int time )
{
  if( !_d->time.start )
  {
    _d->time.start = time;
  }

  if( _d->isActive && ( time - _d->time.start > _d->time.interval ) )
  {
    emit _d->signal.onTimeoutA( _d->id );
    emit _d->signal.onTimeout();

    _d->isActive = false;

    if( _d->loop )
    {
      _d->time.start = time;
      _d->isActive = true;
    }
  }
}

void Timer::setInterval( unsigned int time ) {  _d->time.interval = time;}
void Timer::setLoop( bool loop ) {  _d->loop = loop;}
int Timer::id() const { return _d->id; }
Signal1<int>& Timer::onTimeoutA(){  return _d->signal.onTimeoutA;}
Signal0<>& Timer::onTimeout(){  return _d->signal.onTimeout;}
bool Timer::isActive() const{  return _d->isActive;}
void Timer::destroy(){  _d->isActive = false; }

class DebugTimer::Impl
{
public:
  struct TimerInfo
  {
    std::string name;
    unsigned int time;

    TimerInfo() : time( 0 ) {}
  };

  std::map<unsigned int, TimerInfo> timers;
};

unsigned int DebugTimer::ticks()
{
  return SDL_GetTicks();
}

void DebugTimer::reset(const std::string &name)
{
  unsigned int namehash = Hash( name );
  instance()._d->timers[ namehash ].time = SDL_GetPerformanceCounter();
}

unsigned int DebugTimer::take(const std::string &name, bool reset)
{
  unsigned int namehash = Hash( name );
  Impl::TimerInfo& tinfo = instance()._d->timers[ namehash ];

  unsigned int ret = tinfo.time;
  if( reset )
    tinfo.time = SDL_GetPerformanceCounter();

  return ret;
}

unsigned int DebugTimer::delta(const std::string &name, bool reset)
{
  unsigned int namehash = Hash( name );
  Impl::TimerInfo& tinfo = instance()._d->timers[ namehash ];

  unsigned int ret = SDL_GetPerformanceCounter() - tinfo.time;
  if( reset )
    tinfo.time = SDL_GetPerformanceCounter();

  return ret;
}

void DebugTimer::check(const std::string& prefix, const std::string &name)
{
#ifdef CAESARIA_USE_DEBUGTIMERS
  unsigned int t = delta( name, true );
  Logger::warning( "DEBUG_TIMER:%s%s delta:%d", prefix.c_str(), name.c_str(), t );
#else

#endif
}

DebugTimer& DebugTimer::instance()
{
  static DebugTimer inst;
  return inst;
}

DebugTimer::DebugTimer() : _d(new Impl) {}
