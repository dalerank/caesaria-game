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

#include "loadgame.hpp"
#include "game/game.hpp"
#include "world/empire.hpp"
#include "world/emperor.hpp"
#include "gui/dialogbox.hpp"
#include "game/settings.hpp"
#include "gui/environment.hpp"
#include "core/gettext.hpp"
#include "core/logger.hpp"
#include "gui/loadfiledialog.hpp"
#include "scene/level.hpp"
#include "gui/save_dialog.hpp"

using namespace gui;
using namespace dialog;

namespace events
{

GameEventPtr ShowLoadDialog::create()
{
  GameEventPtr ret( new ShowLoadDialog() );
  ret->drop();

  return ret;
}

void ShowLoadDialog::_exec(Game& game, unsigned int)
{
  scene::Level* lvl = safety_cast<scene::Level*>( game.scene() );
  if( !lvl )
    return;

  Widget* parent = game.gui()->rootWidget();

  vfs::Path savesPath = SETTINGS_VALUE( savedir ).toString();
  std::string defaultExt = SETTINGS_VALUE( saveExt ).toString();
  dialog::LoadFile* wnd = dialog::LoadFile::create( parent, Rect(), savesPath, defaultExt,-1 );

  CONNECT( wnd, onSelectFile(), lvl, scene::Level::loadStage );
  wnd->setTitle( _("##mainmenu_loadgame##") );
}

bool ShowLoadDialog::_mayExec(Game&, unsigned int) const{ return true; }

ShowLoadDialog::ShowLoadDialog() {}

}
