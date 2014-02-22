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

#include "changehomeoptions.hpp"
#include "game/game.hpp"
#include "city/helper.hpp"
#include "city/build_options.hpp"
#include "world/empire.hpp"
#include "game/settings.hpp"

namespace events
{

GameEventPtr ChangeHomeOptions::create()
{
  GameEventPtr ret( new ChangeHomeOptions() );
  ret->drop();

  return ret;
}

void ChangeHomeOptions::load(const VariantMap& stream)
{
  _vars = stream;
}

VariantMap ChangeHomeOptions::save() const
{
  return _vars;
}

bool ChangeHomeOptions::_mayExec(Game& game, unsigned int time) const {  return true; }

void ChangeHomeOptions::_exec(Game& game, unsigned int)
{
  //Variant check_desir = _vars.get( "check_desirability" );
  //if( check_desir.isValid() )
  //{
    //game.getEmpire()->setAvailable( check_desir );
  //}

  //CityHelper helper( game.getCity() );
  //HouseList houses = helper.find<House>( )
}

}
