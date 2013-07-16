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

#ifndef __OPENCAESAR3_TIMER_H_INCLUDED__
#define __OPENCAESAR3_TIMER_H_INCLUDED__

#include "oc3_smartptr.hpp"
#include "oc3_scopedptr.hpp"
#include "oc3_referencecounted.hpp"
#include "oc3_signals.hpp"

class Timer;
typedef SmartPtr< Timer > TimerPtr;

class Timer : public ReferenceCounted
{
public:
  enum { looped=true, singleShot=false };
  static TimerPtr create( unsigned int time, bool loop, int id=-1 );

  ~Timer();

  void update( unsigned int time );

  void setTime( unsigned int time );
  void setLoop( bool loop );

  bool isActive() const;
  
  void destroy();

oc3_signals public:
  Signal1<int>& onTimeoutA();
  Signal0<>& onTimeout();

private:
  Timer();

  class Impl;
  ScopedPtr< Impl > _d;
};

#endif //__OPENCAESAR3_TIMER_H_INCLUDED__

