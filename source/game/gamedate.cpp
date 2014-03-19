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

class GameDate::Impl
{
public:
  DateTime lastDateUpdate;
  DateTime current;
};

DateTime GameDate::current() {  return instance()._d->current; }

GameDate& GameDate::instance()
{
  static GameDate inst;
  return inst;
}

unsigned int GameDate::ticksInMonth() {  return 500; }

void GameDate::timeStep( unsigned int time )
{
  unsigned int dftime = time % ticksInMonth();
  DateTime& date = instance()._d->current;
  if( dftime == 1 )
  {
    // every X seconds    
    date.appendMonth( 1 );
  }  
  date.setDay( (dftime * date.getDaysInMonth() ) / ticksInMonth() );
}

void GameDate::init( const DateTime& date )
{
  instance()._d->current = date;
  instance()._d->lastDateUpdate = date;
}

GameDate::GameDate() : _d( new Impl )
{
  _d->current = DateTime( -350, 0, 0 );
}

GameDate::~GameDate(){}
