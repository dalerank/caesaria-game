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
#include "objects/construction.hpp"
#include "clearland.hpp"
#include "core/logger.hpp"

using namespace gui::dialog;

namespace events
{

GameEventPtr RequestDestroy::create( OverlayPtr constr )
{
  RequestDestroy* e = new RequestDestroy();
  e->_overlay = constr;
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
    std::string constrQuestion = _overlay->errorDesc();
    std::string questionStr = constrQuestion.empty()
                                  ? "##destroy_this_building##"
                                  : constrQuestion;
    Dialog* dialog = Confirmation( game.gui(), _("##warning##"), _(questionStr) );
    CONNECT(dialog, onOk(), this, RequestDestroy::_applyDestroy );
    CONNECT(dialog, onCancel(), this, RequestDestroy::_declineDestroy );
  }
}

bool RequestDestroy::isDeleted() const { return _mayDelete; }
bool RequestDestroy::_mayExec(Game& , unsigned int ) const { return !_alsoExec; }

void RequestDestroy::_applyDestroy()
{
  if( _overlay.isValid() )
  {
    _overlay->setState( pr::destroyable, 1. );
    events::dispatch<ClearTile>( _overlay->pos() );
    _mayDelete = true;
  }
}

void RequestDestroy::_declineDestroy() { _mayDelete = true; }

}
