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

#include "changeempireoptions.hpp"
#include "game/game.hpp"
#include "city/city.hpp"
#include "city/build_options.hpp"
#include "world/empire.hpp"
#include "game/settings.hpp"

namespace events
{

GameEventPtr ChangeEmpireOptions::create()
{
  ChangeEmpireOptions* e = new ChangeEmpireOptions();

  GameEventPtr ret( e );
  ret->drop();

  return ret;
}

void ChangeEmpireOptions::load(const VariantMap& stream)
{
  _vars = stream;
}

bool ChangeEmpireOptions::_mayExec(Game& game, unsigned int time) const {  return true; }

void ChangeEmpireOptions::_exec(Game& game, unsigned int)
{
  VariantMap em_opts = _vars.get( "empire" ).toMap();
  VariantMap adv_options = _vars.get( "adviser" ).toMap();

  Variant empire_enabled = em_opts.get( "enabled" );
  if( empire_enabled.isValid() )
  {
    game.empire()->setAvailable( empire_enabled );
  }

  Variant adv_enabled = adv_options.get( "enabled" );
  if( adv_enabled.isValid() )
  {
    GameSettings::set( GameSettings::adviserEnabled, adv_enabled );
  }
}

}
