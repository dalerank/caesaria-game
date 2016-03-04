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

#include "debug_timer.hpp"
#include "core/utils.hpp"
#include "core/logger.hpp"
#include <SDL_cpuinfo.h>
#include <SDL_timer.h>

class DebugTimer::Impl
{
public:
  struct TimerInfo
  {
    std::string name;
    uint64_t time;

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
  instance()._d->timers[ namehash ].time = static_cast<uint32_t>(SDL_GetPerformanceCounter());
}

uint64_t DebugTimer::take(const std::string &name, bool reset)
{
  unsigned int namehash = Hash( name );
  Impl::TimerInfo& tinfo = instance()._d->timers[ namehash ];

  uint64_t ret = tinfo.time;
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
    tinfo.time = static_cast<uint64_t>(SDL_GetPerformanceCounter());

  return ret;
}

void DebugTimer::check(const std::string& prefix, const std::string &name)
{
#ifdef CAESARIA_USE_DEBUGTIMERS
  unsigned int t = delta( name, true );
  Logger::warning( "DEBUG_TIMER:{0}{1} delta:{2}", prefix, name, t );
#else

#endif
}

DebugTimer& DebugTimer::instance()
{
  static DebugTimer inst;
  return inst;
}

DebugTimer::DebugTimer() : _d(new Impl) {}
