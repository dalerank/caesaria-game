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

#include "gamedate.hpp"

DateTime GameDate::current() {  return instance()._current; }

GameDate& GameDate::instance()
{
  static GameDate inst;
  return inst;
}

unsigned int GameDate::ticksInMonth() {  return 500; }

void GameDate::timeStep( unsigned int time )
{
  unsigned int dftime = time % ticksInMonth();
  if( time > _nextTickChange )
  {
    _nextTickChange += ticksInMonth();
    // every X seconds    
    _current.appendMonth();
  }  
  _current.setDay( (dftime * _current.daysInMonth()) / ticksInMonth() );
}

void GameDate::init( const DateTime& date )
{
  _current = date;
  _nextTickChange = ticksInMonth();
}

GameDate::GameDate()
{
  _current = DateTime( -350, 0, 0 );
}

GameDate::~GameDate(){}
