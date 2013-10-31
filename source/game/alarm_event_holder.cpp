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

#include "alarm_event_holder.hpp"
#include "timer.hpp"
#include "core/stringhelper.hpp"
#include "core/logger.hpp"
#include <vector>

class AlarmEvent
{
public:
  std::string message;
  TilePos position;
};

typedef std::vector< AlarmEvent > AlarmEvents;

class AlarmEventHolder::Impl
{
public:
   AlarmEvents alarms;
   unsigned int currentIndex;
   TimerPtr alarmDeleter;

   void popFront()
   {
     if( alarms.empty() )
     {
       currentIndex = 0;
       return;
     }

     alarms.erase( alarms.begin() );
     currentIndex = math::clamp<int>( currentIndex, 0, alarms.size() - 1 );
     onAlarmChangeSignal.emit( alarms.size() > 0 );
   }

oc3_signals public:
   Signal1<bool> onAlarmChangeSignal;
   Signal1<const TilePos& > onMoveToAlarmSignal;
};

AlarmEventHolder::AlarmEventHolder() : _d( new Impl )
{
  _d->alarmDeleter = Timer::create( 3000, Timer::looped );
  CONNECT( _d->alarmDeleter, onTimeout(), _d.data(), Impl::popFront );
}

AlarmEventHolder::~AlarmEventHolder()
{
  _d->alarmDeleter->destroy();
}

void AlarmEventHolder::add( const TilePos& pos, const std::string& message )
{
  AlarmEvent alarm;
  alarm.message = message;
  alarm.position = pos;
  
  _d->alarms.push_back( alarm );
  _d->onAlarmChangeSignal.emit( _d->alarms.size() > 0 );
}

void AlarmEventHolder::next()
{
  if( _d->alarms.empty() )
    return;

  _d->currentIndex = (_d->currentIndex+1) % _d->alarms.size();
  _d->onMoveToAlarmSignal.emit( _d->alarms.at( _d->currentIndex ).position );
}

TilePos AlarmEventHolder::getCurrentPos() const
{
  if( _d->currentIndex >= _d->alarms.size() )
    return TilePos( -1, -1 );

  return _d->alarms.at( _d->currentIndex ).position;
}

bool AlarmEventHolder::haveAlarms() const
{
  return !_d->alarms.empty();
}

std::string AlarmEventHolder::getCurrentMessage() const
{
  if( _d->currentIndex >= _d->alarms.size() )
    return "";

  return _d->alarms.at( _d->currentIndex ).message;
}

Signal1<bool>& AlarmEventHolder::onAlarmChange()
{
  return _d->onAlarmChangeSignal;
}

Signal1<const TilePos& >& AlarmEventHolder::onMoveToAlarm()
{
  return _d->onMoveToAlarmSignal;
}
