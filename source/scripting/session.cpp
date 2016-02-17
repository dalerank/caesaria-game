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

#include "session.hpp"
#include <GameCity>
#include <GameApp>
#include <GameObjects>
#include <GameScene>
#include <GameGfx>
#include <GameGood>
#include <GameCore>
#include "sound/engine.hpp"
#include "core/osystem.hpp"
#include "steam.hpp"
#include <string>

class Game;
class VariantList;

namespace script
{

void Session::continuePlay(int years)
{
  city::VictoryConditions vc;
  vc = _game->city()->victoryConditions();
  vc.addReignYears( years );

  _game->city()->setVictoryConditions( vc );
}

int Session::lastChangesNum()
{
  return game::Settings::findLastChanges();
}

StringArray Session::getCredits()
{
  StringArray strs;
#define _X(a) strs << a;
#include "core/credits.in"
#undef _X

  return strs;
}

void Session::playAudio(const std::string& filename, int volume, const std::string& mode)
{
  audio::SoundType type = audio::unknown;
  if( mode == "theme" )
    type = audio::theme;
  audio::Engine::instance().play( filename, volume, type );
}

int Session::videoModesCount() { return _game->engine()->modes().size(); }
Size Session::getVideoMode(int index) { return _game->engine()->modes().at(index); }
void Session::setResolution(Size size){SETTINGS_SET_VALUE(resolution, size);}
Size Session::getResolution() { return _game->engine()->screenSize(); }
void Session::saveSettings() { game::Settings::save(); }

StringArray Session::tradableGoods()
{
  return good::tradable().names();
}

VariantMap Session::getGoodInfo(const std::string& goodName)
{
  VariantMap ret;
  good::Info info(good::toType(goodName));
  ret[ "name" ] = info.utname();
  ret[ "picture" ] = info.picture().name();
  ret[ "empPicture" ] = info.picture(true).name();
  ret[ "exportPrice" ] = info.price( _game->city(), good::Info::exporting );
  ret[ "importPrice" ] = info.price( _game->city(), good::Info::importing );

  return ret;
}

void Session::loadNextMission()
{
  city::VictoryConditions vc;
  vc = _game->city()->victoryConditions();
  scene::Level* level = safety_cast<scene::Level*>(_game->scene());
  if( level )
    level->loadStage( vc.nextMission() );
}

void Session::quitGame()
{
  scene::Level* level = safety_cast<scene::Level*>(_game->scene());
  if( level )
    level->quit();
}

void Session::startCareer()
{
  scene::Lobby* lobby = safety_cast<scene::Lobby*>(_game->scene());
  if( lobby )
    lobby->newGame();
}

void Session::openUrl(const std::string& url)
{
  OSystem::openUrl( url, steamapi::ld_prefix() );
}


} //end namespace script
