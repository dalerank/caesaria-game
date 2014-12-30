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
// Copyright 2012-2014 Dalerank, dalerankn8@gmail.com

#ifndef __CAESARIA_GAMEDATE_H_INCLUDED__
#define __CAESARIA_GAMEDATE_H_INCLUDED__
  
#include "core/time.hpp"
#include "core/scopedptr.hpp"

namespace game
{

class Date
{
public:
  void timeStep( unsigned int time );

  void init( const DateTime& date );

  static Date& instance();

  static inline DateTime current() { return instance()._current; }
  static inline bool isDayChanged() { return instance()._dayChange; }
  static inline bool isWeekChanged() { return instance()._weekChange; }
  static inline bool isMonthChanged() { return instance()._monthChange; }
  static inline bool isYearChanged() { return instance()._yearChange; }

  static unsigned int days2ticks( unsigned int days );

private:
  Date();

  DateTime _current;

  //time event flags, it set once by step if availabe, next step reset those
  bool _dayChange;
  bool _weekChange;
  bool _monthChange;
  bool _yearChange;
};

}//end namespace game

#endif //__CAESARIA_GAMEDATE_H_INCLUDED__
