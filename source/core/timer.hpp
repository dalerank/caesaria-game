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

#ifndef __CAESARIA_TIMER_H_INCLUDED__
#define __CAESARIA_TIMER_H_INCLUDED__

#include "core/smartptr.hpp"
#include "core/scopedptr.hpp"
#include "core/referencecounted.hpp"
#include "core/signals.hpp"

class Timer;
typedef SmartPtr< Timer > TimerPtr;

class Timer : public ReferenceCounted
{
public:
  enum { looped=true, singleShot=false };
  static TimerPtr create( unsigned int time, bool loop, int id=-1 );
  static void destroy( int id );

  virtual ~Timer();

  void update( unsigned int time );

  void setInterval( unsigned int time );
  void setLoop( bool loop );

  int id() const;

  bool isActive() const;
  
  void destroy();

signals public:
  Signal1<int>& onTimeoutA();
  Signal0<>& onTimeout();

private:
  Timer();

  class Impl;
  ScopedPtr< Impl > _d;
};

#endif //__CAESARIA_TIMER_H_INCLUDED__

