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

#include "requestdestroy.hpp"
#include "sound/engine.hpp"
#include "game/game.hpp"
#include "gui/dialogbox.hpp"
#include "gui/environment.hpp"
#include "gui/gameautopause.hpp"
#include "scene/level.hpp"
#include "core/gettext.hpp"
#include "clearland.hpp"
#include "core/logger.hpp"

namespace events
{

GameEventPtr RequestDestroy::create( ConstructionPtr constr )
{
  RequestDestroy* e = new RequestDestroy();
  e->_reqConstruction = constr;
  e->_alsoExec = false;
  e->_mayDelete = false;

  GameEventPtr ret( e );
  ret->drop();

  return ret;
}

void RequestDestroy::_exec(Game& game, unsigned int)
{
  if( !_alsoExec )
  {
    _alsoExec = true;
    std::string constrQuestion = _reqConstruction->errorDesc();
    std::string questionStr = constrQuestion.empty()
                                  ? "##destroy_this_building##"
                                  : constrQuestion;
    gui::DialogBox* dialog = new gui::DialogBox( game.gui()->rootWidget(), Rect(), _("##warning##"), _(questionStr), gui::DialogBox::btnOkCancel );
    CONNECT(dialog, onOk(), this, RequestDestroy::_applyDestroy );
    CONNECT(dialog, onOk(), dialog, gui::DialogBox::deleteLater );
    CONNECT(dialog, onCancel(), this, RequestDestroy::_declineDestroy );
    CONNECT(dialog, onCancel(), dialog, gui::DialogBox::deleteLater );
  }
}

bool RequestDestroy::isDeleted() const { return _mayDelete; }
bool RequestDestroy::_mayExec(Game& , unsigned int ) const { return !_alsoExec; }

void RequestDestroy::_applyDestroy()
{
  if( _reqConstruction.isValid() )
  {
    _reqConstruction->setState( Construction::destroyable, 1. );
    GameEventPtr e = ClearTile::create( _reqConstruction->pos() );
    e->dispatch();
    _mayDelete = true;
  }
}

void RequestDestroy::_declineDestroy() { _mayDelete = true; }

}
