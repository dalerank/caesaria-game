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
#include <GameGui>
#include <GameCore>
#include "sound/engine.hpp"
#include "core/font.hpp"
#include "walker/name_generator.hpp"
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

int Session::lastChangesNum() const
{
  return game::Settings::findLastChanges();
}

StringArray Session::getCredits() const
{
  StringArray strs;
#define _X(a) strs << a;
#include "core/credits.in"
#undef _X

  return strs;
}

StringArray Session::getFiles(const std::string& dir, const std::string& ext)
{
  auto flist = vfs::Directory(dir).entries()
                                  .filter(vfs::Entries::file | vfs::Entries::extFilter, ext);

  StringArray names;
  for( auto& it : flist )
    names << it.fullpath.toString();

  std::sort( names.begin(), names.end() );

  return names;
}

void Session::playAudio(const std::string& filename, int volume, const std::string& mode)
{
  audio::SoundType type = audio::unknown;
  if( mode == "theme" )
    type = audio::theme;
  audio::Engine::instance().play( filename, volume, type );
}

int Session::videoModesCount() const { return _game->engine()->modes().size(); }
Size Session::getVideoMode(int index) const { return _game->engine()->modes().at(index); }
Size Session::getResolution() const { return _game->engine()->screenSize(); }

void Session::setResolution(const Size& size)
{
  SETTINGS_SET_VALUE(resolution, size);
  game::Settings::save();
}

void Session::setFont(const std::string& fontname)
{
  FontCollection::instance().initialize(game::Settings::rcpath().toString(), fontname);
}

void Session::setLanguage(const std::string& lang, const std::string& audio)
{
  SETTINGS_SET_VALUE(language,lang);
  SETTINGS_SET_VALUE(talksArchive,audio);

  Locale::setLanguage( lang );
  NameGenerator::instance().setLanguage( lang );
  audio::Helper::initTalksArchive( audio );
}

StringArray Session::tradableGoods() const
{
  return good::tradable().names();
}

VariantMap Session::getGoodInfo(std::string goodName) const
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
    level->loadStage(vc.nextMission());
}

void Session::setMode(int mode)
{
  scene::Base* scene = _game->scene();
  if(scene)
    scene->setMode(mode);
}

void Session::setOption(const std::string& name, Variant v)
{
  scene::Base* scene = _game->scene();
  if(scene)
    scene->setOption(name,v);
}

void Session::clearUi()
{
  _game->gui()->clear();
}

void Session::loadLocalization(const std::string& name)
{
  Locale::addTranslation(name);
}

void Session::openUrl(const std::string& url)
{
  OSystem::openUrl( url, steamapi::ld_prefix() );
}


} //end namespace script
