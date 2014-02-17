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

#include "changebuildingoptions.hpp"
#include "game/game.hpp"
#include "city/city.hpp"
#include "city/build_options.hpp"

namespace events
{

GameEventPtr ChangeBuildingOptions::create(const VariantMap& options)
{
  ChangeBuildingOptions* e = new ChangeBuildingOptions();
  e->_vars = options;

  GameEventPtr ret( e );
  ret->drop();

  return ret;
}

bool ChangeBuildingOptions::_mayExec(Game& game, uint time) const
{
  return true;
}

void ChangeBuildingOptions::_exec(Game& game, uint)
{
  CityBuildOptions options;
  options = game.getCity()->getBuildOptions();
  options.load( _vars );

  game.getCity()->setBuildOptions( options );
}

}
