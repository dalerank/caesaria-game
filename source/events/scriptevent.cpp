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

#include "scriptevent.hpp"
#include "goodrequestevent.hpp"

namespace events
{

GameEventPtr ScriptEvent::create( const VariantMap& stream )
{
  std::string type = stream.get( "type" ).toString();
  if( "good_request" == type ) { return GoodRequestEvent::create( stream ); }

  return GameEventPtr();
}

void ScriptEvent::exec(Game&) {}

}
