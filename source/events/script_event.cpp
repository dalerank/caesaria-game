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

#include "script_event.hpp"
#include "game/game.hpp"
#include "steam.hpp"
#include "city/city.hpp"
#include "city/victoryconditions.hpp"
#include "scripting/core.hpp"
#include "core/variant_list.hpp"

namespace events
{

GameEventPtr SciptFunc::create(const std::string& funcname,
                               const VariantList& params)
{
  GameEventPtr ret( new SciptFunc(funcname,params) );
  ret->drop();
  return ret;
}

void SciptFunc::_exec(Game& game, unsigned int)
{
  script::Core::execFunction( _funcname, _params );
}

bool SciptFunc::_mayExec(Game&, unsigned int) const{ return true; }

SciptFunc::SciptFunc(const std::string& funcname,
                     const VariantList& params) : _funcname(funcname), _params(params) {}

}//end namespace events
