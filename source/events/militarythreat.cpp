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

#include "militarythreat.hpp"
#include "game/game.hpp"
#include "city/cityservice_military.hpp"
#include "city/statistic.hpp"

namespace events
{

class MilitaryThreat::Impl
{
public:
  int value;
};

GameEventPtr MilitaryThreat::create( int value )
{
  GameEventPtr ret( new MilitaryThreat( value ) );
  ret->drop();

  return ret;
}

bool MilitaryThreat::isDeleted() const { return true; }
bool MilitaryThreat::_mayExec(Game& game, unsigned int time) const {  return true; }

MilitaryThreat::MilitaryThreat( int value ) : _d( new Impl )
{
  _d->value = value;
}

void MilitaryThreat::_exec(Game& game, unsigned int)
{
  city::MilitaryPtr ml = city::statistic::finds<city::Military>( game.city() );

  if( ml.isValid() )
  {
    ml->updateThreat( _d->value );
  }
}

}
