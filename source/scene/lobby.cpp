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
#include <GameLogger>
#include <GameApp>

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
  gui::Lobby* menu;         // menu to display
  Game* game;
  Engine* engine;
  std::string fileMap;
  int result;

  Picture userImage;
  gui::Label* lbSteamName;

public:
  void showCredits();
  void constructorMode();
  void showMapSelectDialog();
  void showSaveSelectDialog();
  void showAdvancedMaterials();
  void showPackageOptions();
  void fitScreenResolution();
  void playMenuSoundTheme();
  void resolveSteamStats();
  void restart();
  void openDlcDirectory(Widget* sender);
  gui::Ui& ui();
};

void Lobby::Impl::showSaveSelectDialog()
{
 /* vfs::Path savesPath = SETTINGS_STR( savedir );

  result = Lobby::loadSavedGame;
  auto& loadGameDialog = ui().add<dialog::LoadGame>( savesPath );
  loadGameDialog.setShowExtension( false );
  loadGameDialog.setMayDelete( true );

  CONNECT( &loadGameDialog, onSelectFile(), this, Impl::selectFile );
  loadGameDialog.setTitle( _("##mainmenu_loadgame##") );
  loadGameDialog.setText( _("##load_this_game##") );

  changePlayerNameIfNeed(); */
}

void Lobby::Impl::fitScreenResolution()
{
  gfx::Engine::Modes modes = game->engine()->modes();
  SETTINGS_SET_VALUE( resolution, modes.front() );
  SETTINGS_SET_VALUE( fullscreen, true );
  SETTINGS_SET_VALUE( screenFitted, true );
  game::Settings::save();

  dialog::Information( &ui(), "", "Enabled fullscreen mode. Please restart game");
}

void Lobby::Impl::playMenuSoundTheme()
{
  audio::Engine::instance().play( "main_menu", 50, audio::theme );
}

void Lobby::Impl::resolveSteamStats()
{
  if( steamapi::available() )
  {
    int offset = 0;
    for( int k=0; k < steamapi::achv_count; k++ )
    {
      auto achieventId = steamapi::AchievementType(k);
      if( steamapi::isAchievementReached( achieventId ) )
      {
        gfx::Picture pic = steamapi::achievementImage( achieventId );
        if( pic.isValid() )
        {
          auto& img = ui().add<gui::Image>( Point( 10, 100 + offset ), pic );
          img.setTooltipText( steamapi::achievementCaption( achieventId ) );
          offset += 65;
        }
      }
    }
  }
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

void Lobby::Impl::openDlcDirectory(Widget* sender)
{
  if( sender == 0 )
    return;

  vfs::Path path( sender->getProperty( "path" ).toString() );
  ui().add<DlcFolderViewer>( path );
}

void Lobby::Impl::showPackageOptions()
{
  ui().add<dialog::PackageOptions>( Rect() );
}

void Lobby::Impl::showCredits()
{
  events::dispatch<events::ScriptFunc>( "OnShowCredits" );
}

#define ADD_MENU_BUTTON( text, slot) { auto& btn = menu->addButton( _(text), -1 ); CONNECT( &btn, onClicked(), this, slot ); }

void Lobby::Impl::constructorMode()
{
 /* auto& loadFileDialog = ui().add<dialog::LoadFile>( Rect(),
                                                     vfs::Path( ":/maps/" ), ".map,.sav,.omap",
                                                     -1 );
  loadFileDialog.setMayDelete( false );

  result = Lobby::loadConstructor;
  CONNECT( &loadFileDialog, onSelectFile(), this, Impl::selectFile );
  loadFileDialog.setTitle( _("##mainmenu_loadmap##") );
  loadFileDialog.setText( _("##start_this_map##") );

  changePlayerNameIfNeed(); */
}

void Lobby::Impl::showAdvancedMaterials()
{
  /*menu->clear();

  vfs::Directory dir( ":/dlc" );
  if( !dir.exist() )
  {
    auto& infoDialog = dialog::Information( menu->ui(), _("##no_dlc_found_title##"), _("##no_dlc_found_text##"));
    infoDialog.show();
    showMainMenu();
    return;
  }

  StringArray excludeFolders;
  excludeFolders << vfs::Path::firstEntry << vfs::Path::secondEntry;
  vfs::Entries::Items entries = dir.entries().items();
  for( auto& it : entries )
  {
    if( it.isDirectory )
    {
      if( excludeFolders.contains( it.name.toString() ) )
        continue;

      vfs::Path path2subdir = it.fullpath;
      std::string locText = "##mainmenu_dlc_" + path2subdir.baseName().toString() + "##";

      auto& btn = menu->addButton( _(locText), -1 );
      btn.addProperty( "path", Variant( path2subdir.toString() ) );
      CONNECT( &btn, onClickedEx(), this, Impl::openDlcDirectory )
    }
  }

  ADD_MENU_BUTTON( "##cancel##", Impl::showMainMenu )*/
}

void Lobby::setMode(int mode)
{
  switch(mode)
  {
  case loadMission:
    _isStopped = true;
    if (!vfs::Path(_d->fileMap).exist())
      Logger::warning("WARNING !!! File to load is empty in Lobby::setMode");
  break;

  case startNewGame:
    _d->result=startNewGame;
    _isStopped=true;
  break;

  case loadSavedGame:
    _d->result = Lobby::loadSavedGame;
    _d->fileMap = SETTINGS_STR(lastGame);
    _isStopped = true;
  break;

  case reloadScreen:
    _d->result = Lobby::reloadScreen;
    _isStopped = true;
  break;

  case closeApplication:
    game::Settings::save();
    _d->result=closeApplication;
    _isStopped=true;
  break;
  }
}

void Lobby::Impl::showMapSelectDialog()
{
  /*auto&& loadFileDialog = ui().add<dialog::LoadFile>( Rect(),
                                                      vfs::Path( ":/maps/" ), ".map,.sav,.omap",
                                                      -1 );
  loadFileDialog.setMayDelete( false );

  result = Lobby::loadMap;
  CONNECT( &loadFileDialog, onSelectFile(), this, Impl::selectFile );
  loadFileDialog.setTitle( _("##mainmenu_loadmap##") );
  loadFileDialog.setText( _("##start_this_map##") );

  changePlayerNameIfNeed();*/
}

Lobby::Lobby( Game& game, Engine& engine ) : _d( new Impl )
{
  _d->bgPicture = Picture::getInvalid();
  _isStopped = false;
  _d->game = &game;
  _d->userImage = Picture::getInvalid();
  _d->engine = &engine;
}

Lobby::~Lobby() {}

void Lobby::draw()
{
  _d->ui().beforeDraw();
  _d->engine->draw(_d->bgPicture, _d->bgOffset);
  _d->ui().draw();

  if( steamapi::available() )
  {
    _d->engine->draw( _d->userImage, Point( 20, 20 ) );
  }
}

void Lobby::handleEvent(NEvent& event)
{
  if (event.EventType == sEventQuit)
    setMode(closeApplication);

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
  Logger::warning( "ScreenMenu: initialize start");
  std::string resName = SETTINGS_STR( titleResource );
  _d->bgPicture.load( resName, 1);

  // center the bgPicture on the screen
  Size tmpSize = (_d->ui().vsize() - _d->bgPicture.size())/2;
  _d->bgOffset = Point( tmpSize.width(), tmpSize.height() );

  _d->ui().clear();

  _d->menu = &_d->ui().add<gui::Lobby>();

  events::dispatch<events::ScriptFunc>( "OnLobbyStart" );

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
      CONNECT( &dialog, onNo(), &dialog, Dialogbox::deleteLater );
      CONNECT( &dialog, onYes(),     _d.data(), Impl::fitScreenResolution );
      SETTINGS_SET_VALUE(screenFitted, true);

      dialog.show();
    }
  }

  if( !OSystem::isAndroid() )
    _d->playMenuSoundTheme();

  if( steamapi::available() )
  {
    steamapi::init();

    std::string steamName = steamapi::userName();

    std::string lastName = SETTINGS_STR( playerName );
    if( lastName.empty() )
      SETTINGS_SET_VALUE( playerName, Variant( steamName ) );

    _d->userImage = steamapi::userImage();
    if (steamName.empty())
    {
      OSystem::error( "Error", "Can't login in Steam" );
      _isStopped = true;
      _d->result = closeApplication;
      return;
    }

    std::string text = fmt::format( "Build {0}\n{1}", GAME_BUILD_NUMBER, steamName );
    _d->lbSteamName = &_d->ui().add<Label>( Rect( 100, 10, 400, 80 ), text );
    _d->lbSteamName->setTextAlignment( align::upperLeft, align::center );
    _d->lbSteamName->setWordwrap( true );
    _d->lbSteamName->setFont( FONT_3, ColorList::white );
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
      _d->resolveSteamStats();
  }
}

int Lobby::result() const { return _d->result;}
Ui& Lobby::Impl::ui() { return *game->gui(); }
std::string Lobby::mapName() const { return _d->fileMap;}
std::string Lobby::playerName() const { return SETTINGS_STR( playerName ); }

}//end namespace scene
