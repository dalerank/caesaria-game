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

#include "core/gettext.hpp"
#include "gui/loadgamedialog.hpp"
#include "gfx/engine.hpp"
#include "core/exception.hpp"
#include "gui/startmenu.hpp"
#include "gui/environment.hpp"
#include "game/game.hpp"
#include "game/player.hpp"
#include "gui/pushbutton.hpp"
#include "gui/label.hpp"
#include "game/settings.hpp"
#include "core/color_list.hpp"
#include "gui/playername_window.hpp"
#include "core/logger.hpp"
#include "core/foreach.hpp"
#include "vfs/directory.hpp"
#include "gui/fade.hpp"
#include "gui/listbox.hpp"
#include "core/locale.hpp"
#include "core/saveadapter.hpp"
#include "gui/smkviewer.hpp"
#include "gui/dialogbox.hpp"
#include "core/osystem.hpp"
#include "gui/texturedbutton.hpp"
#include "sound/engine.hpp"
#include "events/setvideooptions.hpp"
#include "events/setsoundoptions.hpp"
#include "gui/widgetpositionanimator.hpp"
#include "gui/loadmissiondialog.hpp"
#include "gui/widgetescapecloser.hpp"
#include "core/event.hpp"
#include "gui/package_options_window.hpp"
#include "core/timer.hpp"
#include "core/variant_map.hpp"
#include "events/dispatcher.hpp"
#include "core/utils.hpp"
#include "walker/name_generator.hpp"
#include "gui/image.hpp"
#include "vfs/directory.hpp"
#include "gui/dlc_folder_viewer.hpp"
#include "steam.hpp"
#include "gui/changes_window.hpp"
#include "gui/window_language_select.hpp"

using namespace gfx;
using namespace gui;

namespace scene
{

namespace internal
{
	static bool wasChangesShow = false;
}

class Lobby::Impl
{
public:
  Picture bgPicture;
  Point bgOffset;
  gui::Lobby* menu;         // menu to display
  bool isStopped;
  Game* game;
  Engine* engine;
  std::string fileMap;
  int result;

  Picture userImage;
  gui::Label* lbSteamName;

public:
  void handleNewGame();
  void showCredits();
  void showLoadMenu();
  void showNewGame();
  void showOptionsMenu();
  void showChangesWindowIfNeed();
  void playRandomap();
  void constructorMode();
  void showMainMenu();
  void showSoundOptions();
  void showVideoOptions();
  void showMissionSelector();
  void quitGame();
  void selectFile( std::string fileName );
  void setPlayerName( std::string name );
  void openSteamPage();
  void openHomePage();
  void showMapSelectDialog();
  void showSaveSelectDialog();
  void changePlayerName();
  void showAdvancedMaterials();
  void startCareer();
  void showLanguageOptions();
  void showPackageOptions();
  void changeLanguage(std::string lang, std::string newFont, std::string sounds);
  void fitScreenResolution();
  void playMenuSoundTheme();
  void continuePlay();
  void resolveSteamStats();
  void changePlayerNameIfNeed(bool force=false);
  void reload();
  void restart();
  void openDlcDirectory(Widget* sender);
  void showLogFile();
  void showChanges();
  gui::Ui& ui();
};

void Lobby::Impl::showSaveSelectDialog()
{
  vfs::Path savesPath = SETTINGS_STR( savedir );

  result = Lobby::loadSavedGame;
  auto& loadGameDialog = ui().add<dialog::LoadGame>( savesPath );
  loadGameDialog.setShowExtension( false );
  loadGameDialog.setMayDelete( true );

  CONNECT( &loadGameDialog, onSelectFile(), this, Impl::selectFile );
  loadGameDialog.setTitle( _("##mainmenu_loadgame##") );
  loadGameDialog.setText( _("##load_this_game##") );

  changePlayerNameIfNeed();
}

void Lobby::Impl::changePlayerName() { changePlayerNameIfNeed(true); }

void Lobby::Impl::showLogFile()
{
  vfs::Directory logfile = SETTINGS_STR( workDir );
  logfile = logfile/SETTINGS_STR( logfile );
  OSystem::openUrl( logfile.toString(), steamapi::ld_prefix() );
}

void Lobby::Impl::showChanges()
{
  SETTINGS_SET_VALUE(showLastChanges, true);
  SETTINGS_SET_VALUE(lastChangesNumber, 0 );
  showChangesWindowIfNeed();
}

void Lobby::Impl::showChangesWindowIfNeed()
{
  int lastChanges = game::Settings::findLastChanges();
  int currentChanges = SETTINGS_VALUE(lastChangesNumber);
  SETTINGS_SET_VALUE(lastChangesNumber, lastChanges );

  if( lastChanges != currentChanges )
    SETTINGS_SET_VALUE(showLastChanges, true);

  if( !internal::wasChangesShow && KILLSWITCH(showLastChanges) )
  {
    internal::wasChangesShow = true;
    game->gui()->add<ChangesWindow>( Rect(0, 0, 500, 500), _("##window_changes_title##"), lastChanges );
  }
}

void Lobby::Impl::changePlayerNameIfNeed(bool force)
{
  std::string playerName = SETTINGS_STR( playerName );
  if( playerName.empty() || force )
  {
    auto& dlg = ui().add<dialog::ChangePlayerName>();
    dlg.setName( playerName );
    dlg.setMayExit( false );

    CONNECT( &dlg, onNameChange(), this, Impl::setPlayerName );
    CONNECT( &dlg, onContinue(), &dlg, dialog::ChangePlayerName::deleteLater );
  }
}

void Lobby::Impl::fitScreenResolution()
{
  gfx::Engine::Modes modes = game->engine()->modes();
  SETTINGS_SET_VALUE( resolution, modes.front() );
  SETTINGS_SET_VALUE( fullscreen, true );
  SETTINGS_SET_VALUE( screenFitted, true );
  game::Settings::save();

  dialog::Information( &ui(), "", "Enabled fullscreen mode. Please restart game");
  //CONNECT( dlg, onOk(), this, Impl::restart );
}

void Lobby::Impl::playMenuSoundTheme()
{
  audio::Engine::instance().play( "main_menu", 50, audio::theme );
}

void Lobby::Impl::continuePlay()
{
  result = Lobby::loadSavedGame;
  selectFile( SETTINGS_STR( lastGame ) );
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

void Lobby::Impl::reload()
{
  result = Lobby::reloadScreen;
  isStopped = true;
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

void Lobby::Impl::showSoundOptions()
{
  events::dispatch<events::ChangeSoundOptions>();
}

void Lobby::Impl::showLanguageOptions()
{
  auto& languageSelectDlg = ui().add<dialog::LanguageSelect>( SETTINGS_RC_PATH( langModel ),
                                                              SETTINGS_STR( language ) );
  languageSelectDlg.setDefaultFont( SETTINGS_STR( defaultFont ) );

  CONNECT_LOCAL( &languageSelectDlg, onChange,   Impl::changeLanguage )
  CONNECT_LOCAL( &languageSelectDlg, onContinue, Impl::reload         )
}

void Lobby::Impl::showPackageOptions()
{
  ui().add<dialog::PackageOptions>( Rect() );
}

void Lobby::Impl::changeLanguage(std::string lang, std::string newFont, std::string sounds)
{  
  std::string currentFont = SETTINGS_STR( font );

  SETTINGS_SET_VALUE( language, Variant( lang ) );
  SETTINGS_SET_VALUE( talksArchive, Variant( sounds ) );

  if( currentFont != newFont )
  {
    SETTINGS_SET_VALUE( font, newFont );
    FontCollection::instance().initialize( game::Settings::rcpath().toString(), newFont );
  }

  game::Settings::save();

  Locale::setLanguage( lang );
  NameGenerator::instance().setLanguage( lang );
  audio::Helper::initTalksArchive( sounds );
}

void Lobby::Impl::startCareer()
{
  menu->clear();

  std::string playerName = SETTINGS_STR( playerName );

  auto& selectPlayerNameDlg = ui().add<dialog::ChangePlayerName>();
  selectPlayerNameDlg.setName( playerName );

  CONNECT_LOCAL( &selectPlayerNameDlg, onNameChange(), Impl::setPlayerName );
  CONNECT_LOCAL( &selectPlayerNameDlg, onContinue(),   Impl::handleNewGame );
  CONNECT_LOCAL( &selectPlayerNameDlg, onClose(),      Impl::showMainMenu  );
}

void Lobby::Impl::handleNewGame()
{  
  result=startNewGame; isStopped=true;
}

void Lobby::Impl::showCredits()
{
  audio::Engine::instance().play( "combat_long", 50, audio::theme );

  StringArray strs;
#define _X(a) strs << a;
#include "core/credits.in"
#undef _X
 
  Size size = ui().vsize();
  Fade& frame = ui().add<Fade>( 0xA0 );
  WidgetClose::insertTo( &frame, KEY_RBUTTON );
  int h = size.height();
  for( int i=0; !strs[i].empty(); i++ )
  {
    Label& lb = frame.add<Label>( Rect( 0, h + i * 20, size.width(), h + (i + 1) * 20), strs[i] );
    lb.setTextAlignment( align::center, align::center );
    lb.setFont( FONT_2_WHITE );
    lb.setSubElement( true );
    auto& animator = lb.add<PositionAnimator>( WidgetAnimator::removeSelf | WidgetAnimator::removeParent, Point( 0, -20), 10000 );
    animator.setSpeed( PointF( 0, -0.5 ) );
  }

  auto& buttonClose = frame.add<PushButton>( Rect( size.width() - 150, size.height() - 34, size.width() - 10, size.height() - 10 ),
                                             _("##close##") );
  frame.setFocus();

  CONNECT( &buttonClose, onClicked(), &frame, Label::deleteLater );
  CONNECT( &buttonClose, onClicked(), this, Impl::playMenuSoundTheme );
}

#define ADD_MENU_BUTTON( text, slot) { auto& btn = menu->addButton( _(text), -1 ); CONNECT( &btn, onClicked(), this, slot ); }

void Lobby::Impl::showLoadMenu()
{
  menu->clear();

  ADD_MENU_BUTTON( "##mainmenu_playmission##", Impl::showMissionSelector )
  ADD_MENU_BUTTON( "##mainmenu_loadgame##",    Impl::showSaveSelectDialog )
  ADD_MENU_BUTTON( "##mainmenu_loadmap##",     Impl::showMapSelectDialog )
  ADD_MENU_BUTTON( "##cancel##",               Impl::showMainMenu )
}

void Lobby::Impl::constructorMode()
{
  auto& loadFileDialog = ui().add<dialog::LoadFile>( Rect(),
                                                     vfs::Path( ":/maps/" ), ".map,.sav,.omap",
                                                     -1 );
  loadFileDialog.setMayDelete( false );

  result = Lobby::loadConstructor;
  CONNECT( &loadFileDialog, onSelectFile(), this, Impl::selectFile );
  loadFileDialog.setTitle( _("##mainmenu_loadmap##") );
  loadFileDialog.setText( _("##start_this_map##") );

  changePlayerNameIfNeed();

}

void Lobby::Impl::playRandomap()
{
  result = Lobby::loadMission;
  fileMap = ":/missions/random.mission";
  isStopped = true;
}

void Lobby::Impl::showOptionsMenu()
{
  menu->clear();

  ADD_MENU_BUTTON( "##mainmenu_language##", Impl::showLanguageOptions )
  ADD_MENU_BUTTON( "##mainmenu_video##",    Impl::showVideoOptions )
  ADD_MENU_BUTTON( "##mainmenu_sound##",    Impl::showSoundOptions )
  ADD_MENU_BUTTON( "##mainmenu_package##",  Impl::showPackageOptions )
  ADD_MENU_BUTTON( "##mainmenu_plname##",   Impl::changePlayerName )
  ADD_MENU_BUTTON( "##mainmenu_showlog##",  Impl::showLogFile )
  ADD_MENU_BUTTON( "##mainmenu_changes##",  Impl::showChanges )
  ADD_MENU_BUTTON( "##cancel##",            Impl::showMainMenu )
}

void Lobby::Impl::showNewGame()
{
  menu->clear();

  ADD_MENU_BUTTON( "##mainmenu_startcareer##", Impl::startCareer )
  ADD_MENU_BUTTON( "##mainmenu_randommap##",   Impl::playRandomap )
  ADD_MENU_BUTTON( "##mainmenu_constructor##", Impl::constructorMode )
  ADD_MENU_BUTTON( "##cancel##",               Impl::showMainMenu )
}

void Lobby::Impl::showMainMenu()
{
  menu->clear();

  std::string lastGame = SETTINGS_STR( lastGame );
  if( !lastGame.empty() )
    ADD_MENU_BUTTON( "##mainmenu_continueplay##", Impl::continuePlay )

  ADD_MENU_BUTTON( "##mainmenu_newgame##",        Impl::showNewGame )
  ADD_MENU_BUTTON( "##mainmenu_load##",           Impl::showLoadMenu )
  ADD_MENU_BUTTON( "##mainmenu_options##",        Impl::showOptionsMenu )
  ADD_MENU_BUTTON( "##mainmenu_credits##",        Impl::showCredits )

  if( vfs::Path( ":/dlc" ).exist() )
    ADD_MENU_BUTTON( "##mainmenu_mcmxcviii##",    Impl::showAdvancedMaterials )

  ADD_MENU_BUTTON( "##mainmenu_quit##",           Impl::quitGame )

  showChangesWindowIfNeed();
}

void Lobby::Impl::showAdvancedMaterials()
{
  menu->clear();

  vfs::Directory dir( ":/dlc" );
  if( !dir.exist() )
  {
    auto infoDialog = dialog::Information( menu->ui(), _("##no_dlc_found_title##"), _("##no_dlc_found_text##"));
    infoDialog->show();
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

  ADD_MENU_BUTTON( "##cancel##", Impl::showMainMenu )
}

void Lobby::Impl::showVideoOptions()
{
  events::dispatch<events::SetVideoSettings>();
}

void Lobby::Impl::showMissionSelector()
{
  result = Lobby::loadMission;
  auto& wnd = ui().add<dialog::LoadMission>( vfs::Path( ":/missions/" ) );

  CONNECT( &wnd, onSelectFile(), this, Impl::selectFile );

  changePlayerNameIfNeed();
}

void Lobby::Impl::quitGame()
{
  game::Settings::save();
  result=closeApplication;
  isStopped=true;
}

void Lobby::Impl::selectFile(std::string fileName)
{
  fileMap = fileName;
  isStopped = true;
}

void Lobby::Impl::setPlayerName(std::string name) { SETTINGS_SET_VALUE( playerName, Variant( name ) ); }
void Lobby::Impl::openSteamPage() { OSystem::openUrl( "http://store.steampowered.com/app/327640", steamapi::ld_prefix() ); }
void Lobby::Impl::openHomePage() { OSystem::openUrl( "http://www.caesaria.net", steamapi::ld_prefix() ); }

void Lobby::Impl::showMapSelectDialog()
{
  auto&& loadFileDialog = ui().add<dialog::LoadFile>( Rect(),
                                                      vfs::Path( ":/maps/" ), ".map,.sav,.omap",
                                                      -1 );
  loadFileDialog.setMayDelete( false );

  result = Lobby::loadMap;
  CONNECT( &loadFileDialog, onSelectFile(), this, Impl::selectFile );
  loadFileDialog.setTitle( _("##mainmenu_loadmap##") );
  loadFileDialog.setText( _("##start_this_map##") );

  changePlayerNameIfNeed();
}

Lobby::Lobby( Game& game, Engine& engine ) : _d( new Impl )
{
  _d->bgPicture = Picture::getInvalid();
  _d->isStopped = false;
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

void Lobby::handleEvent( NEvent& event )
{
  if (event.EventType == sEventQuit)
  {
    _d->quitGame();
  }

  _d->ui().handleEvent( event );
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

  Size scrSize = _d->ui().vsize();
  auto& btnHomePage = _d->ui().add<TexturedButton>( Point( scrSize.width() - 128, scrSize.height() - 100 ), Size( 128 ), -1,
                                                    "logo_rdt", TexturedButton::States( 1, 2, 2, 2 ) );

  auto& btnSteamPage = _d->ui().add<TexturedButton>( Point( btnHomePage.left() - 128, scrSize.height() - 100 ),  Size( 128 ), -1,
                                                     "steam_icon", TexturedButton::States( 1, 2, 2, 2 ) );

  CONNECT( &btnSteamPage, onClicked(), _d.data(), Impl::openSteamPage );
  CONNECT( &btnHomePage, onClicked(), _d.data(), Impl::openHomePage );

  _d->showMainMenu();

  if( OSystem::isAndroid() )
  {
    bool screenFitted = KILLSWITCH( screenFitted ) || KILLSWITCH( fullscreen );
    if( !screenFitted )
    {
      Rect dialogRect = Rect( 0, 0, 400, 150 );
      auto& dialog = _d->ui().add<dialog::Dialog>( dialogRect,
                                                    "Information", "Is need autofit screen resolution?",
                                                    dialog::Dialog::btnOkCancel );
      CONNECT( &dialog, onOk(),     &dialog, dialog::Dialog::deleteLater );
      CONNECT( &dialog, onCancel(), &dialog, dialog::Dialog::deleteLater );
      CONNECT( &dialog, onOk(),     _d.data(), Impl::fitScreenResolution );
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
    if( steamName.empty() )
    {
      OSystem::error( "Error", "Can't login in Steam" );
      _d->isStopped = true;
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

int Lobby::result() const{  return _d->result;}
bool Lobby::isStopped() const{  return _d->isStopped;}
Ui& Lobby::Impl::ui() { return *game->gui(); }
std::string Lobby::mapName() const{  return _d->fileMap;}
std::string Lobby::playerName() const { return SETTINGS_STR( playerName ); }

}//end namespace scene
