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

#include "alarm_event_holder.hpp"
#include "core/timer.hpp"
#include "core/utils.hpp"
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
    emit onAlarmChangeSignal( alarms.size() > 0 );
  }

public signals:
  Signal1<bool> onAlarmChangeSignal;
  Signal1<TilePos> onMoveToAlarmSignal;
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

void AlarmEventHolder::add( TilePos pos, std::string message )
{
  AlarmEvent alarm;
  alarm.message = message;
  alarm.position = pos;
  
  _d->alarms.push_back( alarm );
  emit _d->onAlarmChangeSignal( _d->alarms.size() > 0 );
}

void AlarmEventHolder::next()
{
  if( _d->alarms.empty() )
    return;

  _d->currentIndex = (_d->currentIndex+1) % _d->alarms.size();
  emit _d->onMoveToAlarmSignal( _d->alarms[ _d->currentIndex ].position );
}

TilePos AlarmEventHolder::getCurrentPos() const
{
  if( _d->currentIndex >= _d->alarms.size() )
    return TilePos( -1, -1 );

  return _d->alarms[ _d->currentIndex ].position;
}

bool AlarmEventHolder::haveAlarms() const {  return !_d->alarms.empty(); }

std::string AlarmEventHolder::getCurrentMessage() const
{
  if( _d->currentIndex >= _d->alarms.size() )
    return "";

  return _d->alarms[ _d->currentIndex ].message;
}

Signal1<bool>& AlarmEventHolder::onAlarmChange() {  return _d->onAlarmChangeSignal; }
Signal1<TilePos>& AlarmEventHolder::onMoveToAlarm(){  return _d->onMoveToAlarmSignal;}
