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

#include "warningmessage.hpp"
#include "gui/message_stack_widget.hpp"
#include "game/game.hpp"
#include "city/city.hpp"
#include "gui/environment.hpp"
#include "core/gettext.hpp"

using namespace constants;
using namespace gfx;

namespace events
{


GameEventPtr WarningMessage::create(const std::string& text)
{
  WarningMessage* ev = new WarningMessage();
  ev->_text = text;
  GameEventPtr ret( ev );
  ret->drop();
  return ret;
}

bool WarningMessage::_mayExec(Game& game, unsigned int time) const
{
  return true;
}

void WarningMessage::_exec(Game& game, unsigned int)
{
  if( !game.city()->getOption( PlayerCity::warningsEnabled ) )
    return;

  gui::WindowMessageStack* window = safety_cast<gui::WindowMessageStack*>(
                                      game.gui()->rootWidget()->findChild( gui::WindowMessageStack::defaultID ) );

  if( window && !_text.empty() )
  {
    window->addMessage( _(_text) );
  }
}

} //end namespace events
