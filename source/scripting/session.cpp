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
#include <GameLogger>
#include <GameGfx>
#include <GameGood>
#include <GameGui>
#include <GameCore>
#include <GameEvents>
#include "game/datetimehelper.hpp"
#include "sound/engine.hpp"
#include "core/osystem.hpp"
#include "font/font.hpp"
#include "font/font_collection.hpp"
#include "walker/name_generator.hpp"
#include "steam.hpp"
#include <string>
#include "game/hotkey_manager.hpp"
#include "game/infoboxmanager.hpp"

namespace script
{

static int infoboxLocked = 0;

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

void Session::addWarningMessage(const std::string& message)
{
  events::dispatch<events::WarningMessage>( message, events::WarningMessage::neitral );
}

PlayerPtr Session::getPlayer() const {  return _game->player().object(); }
PlayerCityPtr Session::getCity() const { return _game->city(); }

bool Session::isC3mode() const
{
  return game::Settings::instance().isC3mode();
}

bool Session::isSteamAchievementReached(int i)
{
  if (steamapi::available())
    return steamapi::isAchievementReached(steamapi::AchievementType(i));

  return false;
}

gfx::Picture Session::getSteamUserImage() const
{
  if (steamapi::available())
     return steamapi::userImage();

  return gfx::Picture::getInvalid();
}

gfx::Picture Session::getSteamAchievementImage(int i) const
{
  if (steamapi::available())
    return steamapi::achievementImage(steamapi::AchievementType(i));

  return gfx::Picture::getInvalid();
}

std::string Session::getSteamAchievementCaption(int id) const
{
  if (steamapi::available())
    return steamapi::achievementCaption(steamapi::AchievementType(id));

  return std::string();
}

world::Emperor * Session::getEmperor() const
{
  return &_game->empire()->emperor();
}

world::EmpirePtr Session::getEmpire() const { return _game->empire(); }

void Session::clearHotkeys()
{
  game::HotkeyManager::instance().clear();
}

void Session::setHotkey(const std::string & name, const std::string& config)
{
  game::HotkeyManager::instance().add(name, config);
}

void Session::setRank(int i, const std::string & name, const std::string & pretty, int salary)
{
  world::GovernorRanks& ranks = world::EmpireHelper::ranks();
  if ((int)ranks.size() <= i)
    ranks.resize(i + 1);

  ranks[i].title = name;
  ranks[i].pretty = pretty;
  ranks[i].salary = salary;
  ranks[i].level = (world::GovernorRank::Level)i;
}

DateTime Session::getGameDate() const
{
  return _game->date();
}

std::string Session::formatDate(DateTime date, bool roman) const
{
  std::string text;
  if (roman)
    text = utils::date2str(RomanDate(date), true);
  else
    text = utils::date2str(date, true);

  return text;
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
  auto names = vfs::Directory(dir).entries()
                                  .filter(vfs::Entries::file | vfs::Entries::extFilter, ext)
                                  .items()
                                  .fullnames();

  std::sort(names.begin(), names.end());
  return names;
}

VariantMap Session::winConditions() const
{
  return _game->city()->victoryConditions().save();
}

StringArray Session::getFolders(const std::string& dir, bool full)
{
  vfs::Directory fdir(dir);
  if (!fdir.exist())
    return StringArray();

  return fdir.entries().items().folders(false);
}

void Session::playAudio(const std::string& filename, int volume, int mode)
{
  if (!audio::isAvailableMode(mode))
  {
    Logger::warning("Audio mode {} not available", mode);
    return;
  }
  audio::Engine::instance().play( filename, volume, (audio::SoundType)mode );
}

int Session::videoModesCount() const { return _game->engine()->modes().size(); }
Size Session::getVideoMode(int index) const { return _game->engine()->modes().at(index); }
Size Session::getResolution() const { return _game->engine()->screenSize(); }

void Session::setResolution(const Size& size)
{
  SETTINGS_SET_VALUE(resolution, size);
  game::Settings::save();
}

void Session::showDlcViewer(const std::string& path)
{
  _game->gui()->add<gui::DlcFolderViewer>( path );
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

Point Session::getCursorPos() const
{
  return _game->gui()->cursorPos();
}

std::string Session::getOverlayType(int i) const
{
  return object::toString(object::Type(i));
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
  auto scene = _game->scene();
  if (scene)
    scene->setOption("nextFile", vc.nextMission());
}

void Session::setMode(int mode)
{
  scene::Base* scene = _game->scene();
  if (scene)
    scene->setMode(mode);
}

void Session::setOption(const std::string& name, Variant v)
{
  scene::Base* scene = _game->scene();
  if (scene)
    scene->setOption(name,v);
}

Variant Session::getOption(std::string name)
{
  scene::Base* scene = _game->scene();
  if (scene)
    return scene->getOption(name);

  return Variant();
}

void Session::showSysMessage(std::string title, std::string message)
{
  OSystem::error(title, message);
}

void Session::clearUi()
{
  _game->gui()->clear();
}

void Session::save(const std::string& path)
{
  _game->save(path);
}

void Session::createIssue(const std::string& type, int value)
{
  econ::Issue::Type vtype = econ::findType(type);
  _game->city()->treasury().resolveIssue( {vtype, value} );
}

void Session::createDir(const std::string & dir)
{
  vfs::Directory::createByPath(dir);
}

int Session::getAdvflag(const std::string & flag) const
{
  int value = 0;
  if (flag == "batching")
  {
    value = gfx::Engine::instance().getFlag(gfx::Engine::batching) > 0;
  }
  else if (flag == "lockwindow")
  {
    value = infoboxLocked;
  }
  else if (flag == "tooltips")
  {
    value = _game->gui()->hasFlag(gui::Ui::showTooltips);
  }
  else if (flag == "metric")
  {
    value = metric::Measure::mode();
  }
  else if (flag == "scrollSpeed")
  {
    value = SETTINGS_VALUE(scrollSpeed);
  }
  else if (flag == "gameSpeed")
  {
    value = _game->timeMultiplier();
  }
  else if (flag == "empireLock")
  {
    value = _game->empire()->isAvailable();
  }
  else
  {
    value = citylayer::DrawOptions::getFlag(flag) ? 1 : 0;
  }

  return value;
}

void Session::setAdvflag(const std::string & flag, int value)
{
  if (flag == "batching")
  {
    gfx::Engine::instance().setFlag(gfx::Engine::batching, value);
  }
  else if (flag == "lockwindow")
  {
    infoboxLocked = value;
    gui::infobox::Manager::instance().setBoxLock(value > 0);
  }
  else if (flag == "tooltips")
  {
    _game->gui()->setFlag(gui::Ui::showTooltips, value);
  }
  else if (flag == "metric")
  {
    metric::Measure::setMode((metric::Measure::Mode)value);
    SETTINGS_SET_VALUE(metricSystem, value);
  }
  else if (flag == "scrollSpeed")
  {
    SETTINGS_SET_VALUE(scrollSpeed, value);
    _game->scene()->camera()->setScrollSpeed(value);
  }
  else if (flag == "gameSpeed")
  {
    _game->setTimeMultiplier(value);
  }
  else if (flag == "gameSpeedTick")
  {
    _game->step(value);
  }
  else if (flag == "empireLock")
  {
    _game->empire()->setAvailable(value > 0);
  }
  else
  {
    citylayer::DrawOptions::takeFlag(flag, value);
  }
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
