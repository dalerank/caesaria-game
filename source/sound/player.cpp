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

#include "player.hpp"
#include "core/time.hpp"
#include "game/settings.hpp"
#include "core/stringarray.hpp"
#include "core/foreach.hpp"
#include "core/saveadapter.hpp"
#include "core/variant_map.hpp"
#include "game/gamedate.hpp"
#include "sound/engine.hpp"

namespace audio
{

typedef StringArray Playlist;

class Player::Impl
{
public:
  Playlist playlist;
  int lastIndex;
};

Player::Player( PlayerCityPtr city )
  : Srvc( city, defaultName()  ), _d( new Impl )
{ 
  vfs::Path path = SETTINGS_RC_PATH( soundThemesModel );

  if( path.exist() )
  {
    _d->playlist = config::load( path ).get( "items" ).toStringArray();
  }

  _d->lastIndex = 0;
}

city::SrvcPtr Player::create( PlayerCityPtr city )
{
  city::SrvcPtr ret( new Player( city ) );
  ret->drop();

  return ret;
}

std::string Player::defaultName() { return "audio_player"; }

void Player::timeStep(const unsigned int time )
{
  if( game::Date::isWeekChanged() )
  {
    if( _d->playlist.empty() )
      return;

    audio::Engine& engine = audio::Engine::instance();
    vfs::Path path = _d->playlist[ _d->lastIndex ];

    if( !engine.isPlaying( path ) )
    {
      _d->lastIndex = (_d->lastIndex+1) % _d->playlist.size();
      path = _d->playlist[ _d->lastIndex ];

      engine.play( path, 100, audio::themeSound );
    }
  }
}

Player::~Player()
{
  audio::Engine& engine = audio::Engine::instance();
  vfs::Path path = _d->playlist[ _d->lastIndex ];
  engine.stop( path );
}

}//end namespace audio
