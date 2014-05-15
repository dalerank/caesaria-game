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
// Copyright 2012-2013 Dalerank, dalerankn8@gmail.com

#include "menu.hpp"

#include "core/gettext.hpp"
#include "gui/loadmapwindow.hpp"
#include "gfx/engine.hpp"
#include "core/exception.hpp"
#include "gui/startmenu.hpp"
#include "gui/environment.hpp"
#include "game/game.hpp"
#include "game/player.hpp"
#include "gui/pushbutton.hpp"
#include "gui/label.hpp"
#include "game/settings.hpp"
#include "gui/playername_window.hpp"
#include "core/logger.hpp"
#include "core/foreach.hpp"
#include "vfs/directory.hpp"
#include "gui/label.hpp"
#include "gui/listbox.hpp"
#include "core/locale.hpp"
#include "core/saveadapter.hpp"
#include "gui/smkviewer.hpp"
#include "gui/dialogbox.hpp"
#include "core/osystem.hpp"
#include "gui/texturedbutton.hpp"

using namespace gfx;

namespace scene
{

class StartMenu::Impl
{
public:
  Picture bgPicture;
  gui::StartMenu* menu;         // menu to display
  int result;
  bool isStopped;
  Game* game;
  Engine* engine;
  std::string fileMap;
  std::string playerName;

  void resolveNewGame();
  void resolveCredits();
  
  void resolvePlayMission();
  void resolveQuitGame() { result=closeApplication; isStopped=true; }

  void resolveSelectFile( std::string fileName )
  {
    fileMap = fileName;
    isStopped = true;
  }

  void setPlayerName( std::string name )
  {
    playerName = name;
  }

  void openGreenlightPage() { OSystem::openUrl( "http://steamcommunity.com/sharedfiles/filedetails/?id=249746982" ); }
  void openHomePage() { OSystem::openUrl( "https://bitbucket.org/dalerank/caesaria/wiki/Home" ); }
  void resolveShowLoadMapWnd();
  void resolveShowLoadGameWnd();
  void resolveChangePlayerName();
  void resolveShowChangeLanguageWindow();
  void resolveChangeLanguage(const gui::ListBoxItem&);
  void fitScreenResolution();
  void reload()
  {
    result = StartMenu::reloadScreen;
    isStopped = true;
  }
};

void StartMenu::Impl::resolveShowLoadGameWnd()
{
  gui::Widget* parent = game->gui()->rootWidget();

  vfs::Path savesPath = GameSettings::get( GameSettings::savedir ).toString();
  std::string defaultExt = GameSettings::get( GameSettings::saveExt ).toString();

  result = StartMenu::loadSavedGame;
  gui::LoadMapWindow* wnd = new gui::LoadMapWindow( parent, Rect(), savesPath, defaultExt,-1 );

  CONNECT( wnd, onSelectFile(), this, Impl::resolveSelectFile );
  wnd->setTitle( _("##mainmenu_loadgame##") );
}

void StartMenu::Impl::fitScreenResolution()
{
  gfx::Engine::Modes modes = game->engine()->modes();
  GameSettings::set( GameSettings::resolution, Variant( modes.front() ) );
  GameSettings::set( GameSettings::screenFitted, true );
  GameSettings::save();
}

void StartMenu::Impl::resolveShowChangeLanguageWindow()
{
  gui::Widget* parent = game->gui()->rootWidget();
  Size rootSize = parent->size();
  Size windowSize( 512, 384 );
  Rect rect( Point( (rootSize - windowSize).width() / 2, ( rootSize - windowSize ).height() / 2),
             windowSize );

  gui::Label* frame = new gui::Label( parent, rect, "", false, gui::Label::bgWhiteFrame );
  gui::ListBox* lbx = new gui::ListBox( frame, Rect( 0, 0, 1, 1 ), -1, true, true );
  gui::PushButton* btn = new gui::PushButton( frame, Rect( 0, 0, 1, 1), "Apply" );

  lbx->setGeometry( RectF( 0.05, 0.05, 0.95, 0.85 ) );
  btn->setGeometry( RectF( 0.1, 0.88, 0.9, 0.94 ) );

  VariantMap languages = SaveAdapter::load( GameSettings::rcpath( GameSettings::langModel ) );
  foreach( it, languages )
  {
    lbx->addItem( it->first );
  }

  CONNECT( lbx, onItemSelected(), this, Impl::resolveChangeLanguage );
  CONNECT( btn, onClicked(), this, Impl::reload );
}

void StartMenu::Impl::resolveChangeLanguage(const gui::ListBoxItem& item)
{
  std::string lang;
  VariantMap languages = SaveAdapter::load( GameSettings::rcpath( GameSettings::langModel ) );
  foreach( it, languages )
  {
    if( item.text() == it->first )
    {
      lang = it->second.toString();
      break;
    }
  }

  GameSettings::set( GameSettings::language, Variant( std::string( lang ) ) );

  Locale::setLanguage( GameSettings::get( GameSettings::language ).toString() );
}

void StartMenu::Impl::resolveChangePlayerName()
{
  gui::WindowPlayerName* dlg = new gui::WindowPlayerName( game->gui()->rootWidget() );

  playerName = dlg->text();
  CONNECT( dlg, onNameChange(), this, Impl::setPlayerName );
  CONNECT( dlg, onClose(), this, Impl::resolveNewGame );
}

void StartMenu::Impl::resolveNewGame()
{  
  result=startNewGame; isStopped=true;
}

void StartMenu::Impl::resolveCredits()
{
  gui::Widget* parent = game->gui()->rootWidget();

  gui::Label* frame = new gui::Label( parent, Rect( 0, 0, 512, 384 ), "", false, gui::Label::bgWhiteFrame );
  frame->setCenter( parent->center() );

  gui::ListBox* lbx = new gui::ListBox( frame, Rect( 0, 0, 1, 1 ), -1, true, true );
  gui::PushButton* btn = new gui::PushButton( frame, Rect( 0, 0, 1, 1), _("##close##") );

  lbx->setGeometry( RectF( 0.05, 0.05, 0.95, 0.85 ) );
  btn->setGeometry( RectF( 0.1, 0.88, 0.9, 0.94 ) );

  gui::ListBoxItem& item = lbx->addItem( _("##developers##") );
  item.setTextAlignment( align::center, align::center );
  lbx->addItem( "dalerank (dalerankn8@gmail.com)" );
  lbx->addItem( "gathanase" );
  lbx->addItem( "gecube (gb12335@gmail.com)" );
  lbx->addItem( "pecunia (pecunia@heavengames.com) game mechanics" );
  lbx->addItem( "tracertong" );
  lbx->addItem( "hellium" );
  lbx->addItem( "pufik6666" );
  lbx->addItem( "andreibranescu" );
  lbx->addItem( "AMDmi3 (amdmi3@amdmi3.ru)" );
  lbx->addItem( "akuskis (???) aqueduct system" );
  lbx->addItem( "Rovanion" );
  lbx->addItem( "nickers (2nickers@gmail.com)" );
  lbx->addItem( "ImperatorPrime" );
  lbx->addItem( "veprbl" );
  lbx->addItem( "ramMASTER" );
  lbx->addItem( "Greg Kennedy(kennedy.greg@gmail.com) smk decoder" );
  gui::ListBoxItem& testers = lbx->addItem( _("##testers##") );
  testers.setTextAlignment( align::center, align::center );
  lbx->addItem( "Radek Liška" );
  lbx->addItem( "Dimitrius" );

  gui::ListBoxItem& thanks_to = lbx->addItem( _("##thanks_to##") );
  thanks_to.setTextAlignment( align::center, align::center );
  lbx->addItem( "doc (doc@nnm.me)");

  CONNECT( btn, onClicked(), frame, gui::Label::deleteLater );
}

void StartMenu::Impl::resolvePlayMission()
{
  gui::Widget* parent = game->gui()->rootWidget();
  Size rootSize = parent->size();
  Size windowSize( 512, 384 );
  Rect rect( Point( (rootSize - windowSize).width() / 2, ( rootSize - windowSize ).height() / 2),
             windowSize );

  result = StartMenu::loadMission;
  gui::LoadMapWindow* wnd = new gui::LoadMapWindow( parent, rect,
                                                    GameSettings::rcpath( "/missions/" ), ".mission", -1 );

  CONNECT( wnd, onSelectFile(), this, Impl::resolveSelectFile );
  wnd->setTitle( _("##mainmenu_playmission##") );
}

void StartMenu::Impl::resolveShowLoadMapWnd()
{
  gui::Widget* parent = game->gui()->rootWidget();

  gui::LoadMapWindow* wnd = new gui::LoadMapWindow( parent,
                                                    Rect(),
                                                    GameSettings::rcpath( "/maps/" ), ".map",
                                                    -1 );

  result = StartMenu::loadMap;
  CONNECT( wnd, onSelectFile(), this, Impl::resolveSelectFile );
  wnd->setTitle( _("##mainmenu_loadmap##") );
}

StartMenu::StartMenu( Game& game, Engine& engine ) : _d( new Impl )
{
  _d->bgPicture = Picture::getInvalid();
  _d->isStopped = false;
  _d->game = &game;
  _d->engine = &engine;
}

StartMenu::~StartMenu() {}

void StartMenu::draw()
{
  _d->game->gui()->beforeDraw();

  _d->engine->draw(_d->bgPicture, 0, 0);

  _d->game->gui()->draw();
}

void StartMenu::handleEvent( NEvent& event ){  _d->game->gui()->handleEvent( event );}

void StartMenu::initialize()
{
  Logger::warning( "ScreenMenu: initialize start");
  _d->bgPicture = Picture::load("title", 1);

  // center the bgPicture on the screen
  Size tmpSize = (_d->engine->screenSize() - _d->bgPicture.size())/2;
  _d->bgPicture.setOffset( Point( tmpSize.width(), -tmpSize.height() ) );

  _d->game->gui()->clear();

  _d->menu = new gui::StartMenu( _d->game->gui()->rootWidget() );

  gui::TexturedButton* btnGreenlight = new gui::TexturedButton( _d->game->gui()->rootWidget(), Point(), Size( 250, 155), -1,
                                                                "greenlight", 1, 2, 2, 2 );
  CONNECT( btnGreenlight, onClicked(), _d.data(), Impl::openGreenlightPage );

  Size scrSize = _d->engine->screenSize();
  gui::TexturedButton* btnHomePage = new gui::TexturedButton( _d->game->gui()->rootWidget(),
                                                              Point( scrSize.width() - 97, scrSize.height() - 75 ), Size( 97, 75 ), -1,
                                                              "logo_rdt", 1, 2, 2, 2 );
  CONNECT( btnHomePage, onClicked(), _d.data(), Impl::openHomePage );

  gui::PushButton* btn = _d->menu->addButton( _("##mainmenu_newgame##"), -1 );
  CONNECT( btn, onClicked(), _d.data(), Impl::resolveChangePlayerName );

  btn = _d->menu->addButton( _("##mainmenu_playmission##"), -1 );
  CONNECT( btn, onClicked(), _d.data(), Impl::resolvePlayMission );

  btn = _d->menu->addButton( _("##mainmenu_loadgame##"), -1 );
  CONNECT( btn, onClicked(), _d.data(), Impl::resolveShowLoadGameWnd );

  btn = _d->menu->addButton( _("##mainmenu_loadmap##"), -1 );
  CONNECT( btn, onClicked(), _d.data(), Impl::resolveShowLoadMapWnd );

  btn = _d->menu->addButton( _("Language"), -1 );
  CONNECT( btn, onClicked(), _d.data(), Impl::resolveShowChangeLanguageWindow );

  btn = _d->menu->addButton( _("Credits"), -1 );
  CONNECT( btn, onClicked(), _d.data(), Impl::resolveCredits );

  btn = _d->menu->addButton( _("##mainmenu_quit##"), -1 );
  CONNECT( btn, onClicked(), _d.data(), Impl::resolveQuitGame );

#ifdef CAESARIA_PLATFORM_ANDROID
  bool screenFitted = GameSettings::get( GameSettings::screenFitted );
  if( !screenFitted )
  {
    gui::DialogBox* dialog = new gui::DialogBox( _d->game->gui()->rootWidget(),  Rect( 0, 0, 400, 150 ),
                                                 "Information", "Is need autofit screen resolution?",
                                                 gui::DialogBox::btnOk | gui::DialogBox::btnCancel );
    CONNECT(dialog, onOk(), dialog, gui::DialogBox::deleteLater );
    CONNECT(dialog, onCancel(), dialog, gui::DialogBox::deleteLater );
    CONNECT(dialog, onOk(), _d.data(), Impl::fitScreenResolution );
    dialog->show();
  }
#endif
}

int StartMenu::result() const{  return _d->result;}
bool StartMenu::isStopped() const{  return _d->isStopped;}
std::string StartMenu::getMapName() const{  return _d->fileMap;}
std::string StartMenu::getPlayerName() const { return _d->playerName; }

}//end namespace scene
