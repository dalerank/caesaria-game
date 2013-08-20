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

#include "oc3_gamedate.hpp"

class GameDate::Impl
{
public:
  DateTime lastDateUpdate;
  DateTime current;

oc3_signals public:
  Signal1<const DateTime&> onMonthChangeSignal;
};


DateTime GameDate::current()
{
  return instance()._d->current;
}

GameDate& GameDate::instance()
{
  static GameDate inst;
  return inst;
}

void GameDate::timeStep( unsigned int time )
{
  if( time % 110 == 1 )
  {
    // every X seconds
    GameDate& inst = instance();
    inst._d->current.appendMonth( 1 );
    inst._d->onMonthChangeSignal.emit( inst._d->current );
  }
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

GameDate::~GameDate()
{

}

Signal1<const DateTime&>& GameDate::onMonthChanged()
{
  return _d->onMonthChangeSignal;
}
