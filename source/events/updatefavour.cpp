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

#include "updatefavour.hpp"
#include "game/game.hpp"
#include "world/empire.hpp"
#include "world/emperor.hpp"

namespace events
{

GameEventPtr UpdateFavour::create(std::string cityname, int value)
{
  UpdateFavour* e = new UpdateFavour();
  e->_cityname = cityname;
  e->_value = value;

  GameEventPtr ret( e );
  ret->drop();

  return ret;
}

void UpdateFavour::_exec(Game& game, unsigned int)
{
  world::Emperor& emperor = game.empire()->emperor();
  emperor.updateRelation( _cityname, _value );
}

bool UpdateFavour::_mayExec(Game&, unsigned int) const{  return true; }

UpdateFavour::UpdateFavour() : _cityname( ""), _value( 0 )
{}

}
