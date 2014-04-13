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

#include "scribemessage.hpp"
#include "game/game.hpp"
#include "game/gamedate.hpp"
#include "city/cityservice_info.hpp"
#include "city/city.hpp"

namespace events
{

GameEventPtr ScribeMessage::create( const std::string& title, const std::string& text )
{
  ScribeMessage* e = new ScribeMessage();
  e->_title = title;
  e->_text = text;

  GameEventPtr ret( e );
  ret->drop();

  return ret;
}

void ScribeMessage::_exec(Game& game, unsigned int)
{
  SmartPtr<city::Info> srvc = ptr_cast<city::Info>( game.city()->findService( city::Info::getDefaultName() ) );
  if( srvc.isValid() )
  {
    city::Info::ScribeMessage msg;
    msg.date = GameDate::current();
    msg.opened = false;
    msg.text = _text;
    msg.title = _title;
    msg.type = 0;
    srvc->addMessage( msg );
  }
}

bool ScribeMessage::_mayExec(Game&, unsigned int) const { return true; }

}//end namespace events
