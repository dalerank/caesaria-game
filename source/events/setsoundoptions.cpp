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
// Copyright 2012-2013 Dalerank, dalerankn8@gmail.com

#include "setsoundoptions.hpp"
#include "gui/sound_options_window.hpp"
#include "sound/engine.hpp"
#include "game/settings.hpp"
#include "game/game.hpp"
#include "core/utils.hpp"
#include "gui/environment.hpp"
#include "core/logger.hpp"

using namespace gui::dialog;
using namespace audio;

namespace events
{

class Model
{
public:
  struct Info
  { audio::Volume volumes[ audio::count ]; };

  Info _current, _save;

  Model()
  {
    Engine& ae = Engine::instance();

    Info tmp;
    for( int i=audio::ambient; i < audio::count; i++ )
      tmp.volumes[ i ] = ae.volume( SoundType(i) );

    _save = tmp;
    _current = tmp;
  }

  void destroy() { delete this; }

  void set( audio::SoundType type, audio::Volume value )
  {
    _current.volumes[ type ] = math::clamp( value, audio::minVolume, audio::maxVolume );
  }

  unsigned int get( audio::SoundType type ) const;

  void apply()
  {
    SETTINGS_SET_VALUE( soundVolume,   _current.volumes[ audio::game ] );
    SETTINGS_SET_VALUE( ambientVolume, _current.volumes[ audio::ambient ] );
    SETTINGS_SET_VALUE( musicVolume,   _current.volumes[ audio::theme ] );
    game::Settings::save();
    destroy();
  }

  void restore()
  {
    Engine& ae = Engine::instance();
    for( int i=audio::ambient; i < audio::count; i++ )
      ae.setVolume( SoundType(i), _save.volumes[ i ] );
    destroy();
  }

};

GameEventPtr ChangeSoundOptions::create()
{
  GameEventPtr ret( new ChangeSoundOptions() );
  ret->drop();

  return ret;
}

void ChangeSoundOptions::_exec(Game& game, unsigned int)
{
  SoundOptions* dialog = new SoundOptions( game.gui()->rootWidget() );
  Model* model = new Model();

  Engine& ae = Engine::instance();

  CONNECT( dialog, onChange(), model,  Model::set )
  CONNECT( dialog, onClose(),  model,  Model::restore )
  CONNECT( dialog, onChange(), &ae,    Engine::setVolume )

  dialog->update( audio::ambient, ae.volume( audio::ambient ) );
  dialog->update( audio::theme, ae.volume( audio::theme ) );
  dialog->update( audio::game, ae.volume( audio::game ) );

  dialog->show();
}

bool ChangeSoundOptions::_mayExec(Game&, unsigned int) const { return true; }

}
