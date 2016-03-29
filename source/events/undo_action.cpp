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
// Copyright 2012-2015 Dalerank, dalerankn8@gmail.com

#include "undo_action.hpp"
#include "scene/level.hpp"
#include "city/undo_stack.hpp"

using namespace gfx;
using namespace city;

namespace events
{

GameEventPtr UndoAction::create(Type event, object::Type type, const TilePos& pos, int money)
{
  UndoAction* action = new UndoAction();
  action->_pos = pos;
  action->_money = money;
  action->_overlayType = type;
  action->_action = event;

  GameEventPtr e( action );
  e->drop();

  return e;
}

bool UndoAction::_mayExec(Game&, unsigned int) const {  return true;}

void UndoAction::_exec( Game& game, unsigned int )
{  
  scene::Level* level = safety_cast<scene::Level*>( game.scene() );
  if( level )
  {
    switch(_action)
    {
    case built:
      level->undoStack().build( _overlayType, _pos, _money );
    break;

    case destroyed:
      level->undoStack().destroy( _overlayType, _pos, _money );
    break;

    case clear:
      level->undoStack().clear();
    break;

    case revert:
      level->undoStack().undo();
    break;

    case finished:
      level->undoStack().finished();
    break;

    default:
    break;
    }
  }
}

} //end namespace events
