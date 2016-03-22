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
// Copyright 2012-2013 Gregoire Athanase, gathanase@gmail.com
// Copyright 2012-2015 Dalerank, dalerankn8@gmail.com

#include "lobby.hpp"

#include "scripting/core.hpp"
#include <GameCore>
#include <GameGui>
#include <GameGfx>
#include <GameEvents>
#include <GameVfs>
#include <GameCity>
#include <GameLogger>
#include <GameApp>
#include "font/font_context.h"

#include "sound/engine.hpp"
#include "steam.hpp"
#include "walker/name_generator.hpp"

using namespace gfx;
using namespace gui;

namespace scene
{

class Lobby::Impl
{
public:
  Picture bgPicture;
  Point bgOffset;
  Game* game;
  Engine* engine;
  std::string fileMap;
  int result;

public:
  void fitScreenResolution();
  void resolveSteamStats();
  void restart();
  gui::Ui& ui();
};

void Lobby::Impl::fitScreenResolution()
{
  gfx::Engine::Modes modes = game->engine()->modes();
  SETTINGS_SET_VALUE(resolution, modes.front());
  SETTINGS_SET_VALUE(fullscreen, true);
  SETTINGS_SET_VALUE(screenFitted, true);
  game::Settings::save();

  dialog::Information( &ui(), "", "Enabled fullscreen mode. Please restart game");
}

void Lobby::Impl::restart()
{
  std::string filename;

  if( OSystem::isLinux() ) filename = "caesaria.linux";
  else if( OSystem::isWindows() ) filename = "caesaria.exe";
  else if( OSystem::isMac() ) filename = "caesaria.macos";
  else filename = "unknown";

  vfs::Directory appDir = vfs::Directory::applicationDir();
  vfs::Path appFile = appDir/vfs::Path(filename);
  OSystem::restartProcess( appFile.toString(), appDir.toString(), StringArray() );
}

void Lobby::setMode(int mode)
{
  switch(mode)
  {
  case loadMission:
    _isStopped = true;
    _d->result = loadMission;
    if (!vfs::Path(_d->fileMap).exist())
      Logger::warning("!!! File to load is empty in Lobby::setMode");
  break;

  case loadMap:
  case loadConstructor:
    _isStopped = true;
    _d->result = mode;
  break;

  case startNewGame:
    _d->result=startNewGame;
    _isStopped=true;
  break;

  case loadSavedGame:
    _d->result = Lobby::loadSavedGame;
    _isStopped = true;
  break;

  case reloadScreen:
    _d->result = Lobby::reloadScreen;
    _isStopped = true;
  break;

  case res_close:
    game::Settings::save();
    _d->result=res_close;
    _isStopped=true;
  break;
  }
}

Lobby::Lobby( Game& game, Engine& engine ) : _d( new Impl )
{
  _d->bgPicture = Picture::getInvalid();
  _isStopped = false;
  _d->game = &game;
  _d->engine = &engine;
}

Lobby::~Lobby() {}

void Lobby::draw(Engine& engine)
{
  _d->ui().beforeDraw();
  _d->engine->draw(_d->bgPicture, _d->bgOffset);
  _d->ui().draw();
}

void Lobby::handleEvent(NEvent& event)
{
  if (event.EventType == sEventQuit)
    setMode(res_close);

  _d->ui().handleEvent( event );
}

void Lobby::setOption(const std::string& name, Variant value)
{
  if(name=="nextFile")
    _d->fileMap = value.toString();
}

void Lobby::initialize()
{
  events::Dispatcher::instance().reset();
  Logger::debug("ScreenMenu: initialize start");
  std::string resName = SETTINGS_STR(titleResource);
  _d->bgPicture.load(resName, 1);

  // center the bgPicture on the screen
  Size tmpSize = (_d->ui().vsize() - _d->bgPicture.size())/2;
  _d->bgOffset = Point(tmpSize.width(), tmpSize.height());

  _d->ui().clear();

  events::dispatch<events::ScriptFunc>("OnLobbyStart");

  if( OSystem::isAndroid() )
  {
    bool screenFitted = KILLSWITCH( screenFitted ) || KILLSWITCH( fullscreen );
    if( !screenFitted )
    {
      Rect dialogRect = Rect( 0, 0, 400, 150 );
      auto& dialog = _d->ui().add<Dialogbox>( dialogRect,
                                              "Information", "Is need autofit screen resolution?",
                                              Dialogbox::btnYesNo );
      CONNECT( &dialog, onYes(),     &dialog, Dialogbox::deleteLater );
      CONNECT( &dialog, onNo(),      &dialog, Dialogbox::deleteLater );
      CONNECT( &dialog, onYes(),     _d.data(), Impl::fitScreenResolution );
      SETTINGS_SET_VALUE(screenFitted, true);

      dialog.show();
    }
  }

  if( steamapi::available() )
  {
    steamapi::init();

    VariantList vl; vl << Variant(steamapi::userName());
    events::dispatch<events::ScriptFunc>("OnReceivedSteamUserName", vl);
  }
}

void Lobby::afterFrame()
{
  Base::afterFrame();

  static unsigned int saveTime = 0;
  events::Dispatcher::instance().update( *_d->game, saveTime++ );

  if( steamapi::available() )
  {
    steamapi::update();
    if( steamapi::isStatsReceived() )
      events::dispatch<events::ScriptFunc>("OnReceivedSteamStats");
  }
}

int Lobby::result() const { return _d->result;}
Ui& Lobby::Impl::ui() { return *game->gui(); }
std::string Lobby::mapName() const { return _d->fileMap;}
std::string Lobby::playerName() const { return SETTINGS_STR( playerName ); }

}//end namespace scene
