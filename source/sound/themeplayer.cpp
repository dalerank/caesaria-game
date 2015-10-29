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
// Copyright 2012-2014 dalerank, dalerankn8@gmail.com

#include "themeplayer.hpp"
#include "core/time.hpp"
#include "game/settings.hpp"
#include "core/stringarray.hpp"
#include "core/foreach.hpp"
#include "core/saveadapter.hpp"
#include "core/variant_map.hpp"
#include "game/gamedate.hpp"
#include "sound/engine.hpp"
#include "city/cityservice_factory.hpp"

namespace city
{
REGISTER_SERVICE_IN_FACTORY(audio::ThemePlayer,themePlayer)
}

namespace audio
{

typedef StringArray Playlist;

class ThemePlayer::Impl
{
public:
  Playlist playlist;
  int lastIndex;
  Signal1<std::string> onSwitchSignal;

  void trySwitch2next();  
};

ThemePlayer::ThemePlayer( PlayerCityPtr city )
  : Srvc( city, defaultName()  ), _d( new Impl )
{ 
  vfs::Path path = SETTINGS_RC_PATH( soundThemesModel );

  if( path.exist() )
  {
    _d->playlist = config::load( path ).get( "items" ).toStringArray();
  }

  _d->lastIndex = 0;
}

Signal1<std::string>& ThemePlayer::onSwitch() { return _d->onSwitchSignal; }

std::string ThemePlayer::defaultName() { return "audio_player"; }

void ThemePlayer::timeStep(const unsigned int time )
{
  if( game::Date::isWeekChanged() )
  {
    if( _d->playlist.empty() )
      return;

    Engine& engine = Engine::instance();
    std::string sample = _d->playlist[ _d->lastIndex ];

    if( !engine.isPlaying( sample ) )
    {
      _d->trySwitch2next();
    }
  }
}

void ThemePlayer::next()
{
  _d->trySwitch2next();
}

ThemePlayer::~ThemePlayer()
{
  Engine& engine = Engine::instance();
  std::string sample = _d->playlist[ _d->lastIndex ];
  engine.stop( sample );
}

}//end namespace audio


void audio::ThemePlayer::Impl::trySwitch2next()
{
  Engine& engine = Engine::instance();
  std::string sample = playlist[ lastIndex ];

  lastIndex = (lastIndex+1) % playlist.size();
  sample = playlist[ lastIndex ];
  engine.play( sample, 100, audio::theme );

  emit onSwitchSignal( "##theme_" + sample + "##" );
}
