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

#include "ability.hpp"

Illness::Illness(int strong, int delay)
{
  _strong = strong;
  _time = 0;
  _delay = delay;
}

AbilityPtr Illness::create(double strong, unsigned int delay)
{
  AbilityPtr ret( new Illness( strong, delay ) );
  ret->drop();

  return ret;
}

void Illness::run(WalkerPtr parent, unsigned int time)
{
  if( _time >= _delay )
  {
    _time = 0;
    parent->updateHealth( -_strong );
  }

  _time++;
}
