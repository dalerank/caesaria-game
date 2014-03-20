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
#include "game/gamedate.hpp"
#include "sound/engine.hpp"

namespace audio
{

typedef StringArray Playlist;

class Player::Impl
{
public:
  Playlist playlist;
  int updateInterval;
  int lastIndex;
};

Player::Player() : Srvc( "audio_player" ), _d( new Impl )
{ 
  vfs::Path path = GameSettings::rcpath( GameSettings::soundThemesModel );

  if( path.exist() )
  {
    _d->playlist = SaveAdapter::load( path ).get( "items" ).toStringArray();
  }

  _d->updateInterval = GameDate::ticksInMonth() / 2;
  _d->lastIndex = 0;
}

city::SrvcPtr Player::create(PlayerCityPtr city)
{
  Player* pl = new Player();
  city::SrvcPtr ret( pl );
  pl->drop();

  return ret;
}

void Player::update( const unsigned int time )
{
  if( time % _d->updateInterval == 1 )
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

Player::~Player() {}

}//end namespace audio
