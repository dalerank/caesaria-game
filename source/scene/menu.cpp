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

  void resolveShowLoadMapWnd();
  void resolveShowLoadGameWnd();
  void resolveChangePlayerName();
  void resolveShowChangeLanguageWindow();
  void resolveChangeLanguage(const gui::ListBoxItem&);
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
  Size rootSize = parent->size();
  Size windowSize( 512, 384 );
  Rect rect( Point( (rootSize - windowSize).width() / 2, ( rootSize - windowSize ).height() / 2),
             windowSize );

  gui::Label* frame = new gui::Label( parent, rect, "", false, gui::Label::bgWhiteFrame );
  gui::ListBox* lbx = new gui::ListBox( frame, Rect( 0, 0, 1, 1 ), -1, true, true );
  gui::PushButton* btn = new gui::PushButton( frame, Rect( 0, 0, 1, 1), "Close" );

  lbx->setGeometry( RectF( 0.05, 0.05, 0.95, 0.85 ) );
  btn->setGeometry( RectF( 0.1, 0.88, 0.9, 0.94 ) );

  gui::ListBoxItem& item = lbx->addItem( _("##developers##") );
  item.setTextAlignment( alignCenter, alignCenter );
  lbx->addItem( "dalerank (dalerankn8@gmail.com)" );
  lbx->addItem( "gathanase" );
  lbx->addItem( "gecube (gb12335@gmail.com)" );
  lbx->addItem( "pecunia (pecunia@heavengames.com) game mechanics" );
  lbx->addItem( "tracertong" );
  lbx->addItem( "hellium" );
  lbx->addItem( "pufik6666" );
  lbx->addItem( "andreibranescu" );
  lbx->addItem( "AMDmi3 (amdmi3@amdmi3.ru)" );
  lbx->addItem( "akuskis () aqueduct system" );
  lbx->addItem( "Rovanion" );
  lbx->addItem( "nickers (2nickers@gmail.com)" );
  lbx->addItem( "ImperatorPrime" );
  lbx->addItem( "veprbl" );
  lbx->addItem( "ramMASTER" );
  lbx->addItem( "Greg Kennedy(kennedy.greg@gmail.com) smk decoder" );
  gui::ListBoxItem& testers = lbx->addItem( _("##testers##") );
  testers.setTextAlignment( alignCenter, alignCenter );
  lbx->addItem( "Radek Liška" );
  lbx->addItem( "Dimitrius" );

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

  _d->engine->drawPicture(_d->bgPicture, 0, 0);

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

  /*gui::SmkViewer* smkv = new gui::SmkViewer( _d->game->gui()->rootWidget(), Rect( 300, 300, 600, 600 ), gui::SmkViewer::video );
  smkv->setFilename( GameSettings::rcpath( "/smk/1C.smk" ) );
  CONNECT( smkv, onFinish(), smkv, gui::SmkViewer::deleteLater );*/
}

int StartMenu::result() const{  return _d->result;}
bool StartMenu::isStopped() const{  return _d->isStopped;}
std::string StartMenu::getMapName() const{  return _d->fileMap;}
std::string StartMenu::getPlayerName() const { return _d->playerName; }

}//end namespace scene
