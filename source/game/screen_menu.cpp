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

#include "screen_menu.hpp"

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
#include "vfs/directory.hpp"
#include "gui/label.hpp"
#include "gui/listbox.hpp"
#include "core/locale.hpp"
#include "core/saveadapter.hpp"

class ScreenMenu::Impl
{
public:
  Picture bgPicture;
  gui::StartMenu* menu;         // menu to display
  int result;
  bool isStopped;
  Game* game;
  GfxEngine* engine;
  std::string fileMap;

  void resolveNewGame();

  void resolveLoadGame( std::string fileName )
  {
    result = ScreenMenu::loadSavedGame;
    fileMap = fileName;
    isStopped=true; 
  }
  
  void resolvePlayMission();
  void resolveQuitGame() { result=closeApplication; isStopped=true; }
  
  void resolveSelectFile( std::string fileName )
  {
    result = ScreenMenu::loadMap;
    fileMap = fileName;
    isStopped = true;
  }

  void setPlayerName( std::string name )
  {
    game->getPlayer()->setName( name );
  }

  void resolveShowLoadMapWnd();
  void resolveShowLoadGameWnd();
  void resolveChangePlayerName();
  void resolveShowChangeLanguageWindow();
  void resolveChangeLanguage(const gui::ListBoxItem&);
  void reload()
  {
    result = ScreenMenu::reloadScreen;
    isStopped = true;
  }
};

void ScreenMenu::Impl::resolveShowLoadGameWnd()
{
  gui::Widget* parent = game->getGui()->getRootWidget();
  Size rootSize = parent->getSize();
  Size windowSize( 512, 384 );
  Rect rect( Point( (rootSize - windowSize).getWidth() / 2, ( rootSize - windowSize ).getHeight() / 2),
             windowSize );

  vfs::Path savesPath = GameSettings::get( GameSettings::savedir ).toString();
  std::string defaultExt = GameSettings::get( GameSettings::saveExt ).toString();

  gui::LoadMapWindow* wnd = new gui::LoadMapWindow( parent, rect, savesPath, defaultExt,-1 );

  CONNECT( wnd, onSelectFile(), this, Impl::resolveSelectFile );
  wnd->setTitle( _("##mainmenu_loadgame##") );
}

void ScreenMenu::Impl::resolveShowChangeLanguageWindow()
{
  gui::Widget* parent = game->getGui()->getRootWidget();
  Size rootSize = parent->getSize();
  Size windowSize( 512, 384 );
  Rect rect( Point( (rootSize - windowSize).getWidth() / 2, ( rootSize - windowSize ).getHeight() / 2),
             windowSize );

  gui::Label* frame = new gui::Label( parent, rect, "", false, gui::Label::bgWhiteFrame );
  gui::ListBox* lbx = new gui::ListBox( frame, Rect( 0, 0, 1, 1 ), -1, true, true );
  gui::PushButton* btn = new gui::PushButton( frame, Rect( 0, 0, 1, 1), "Apply" );

  lbx->setGeometry( RectF( 0.05, 0.05, 0.95, 0.85 ) );
  btn->setGeometry( RectF( 0.1, 0.88, 0.9, 0.94 ) );

  VariantMap languages = SaveAdapter::load( GameSettings::rcpath( GameSettings::langModel ) );
  for( VariantMap::iterator it=languages.begin(); it != languages.end(); it++ )
  {
    lbx->addItem( it->first );
  }

  CONNECT( lbx, onItemSelected(), this, Impl::resolveChangeLanguage );
  CONNECT( btn, onClicked(), this, Impl::reload );
}

void ScreenMenu::Impl::resolveChangeLanguage(const gui::ListBoxItem& item)
{
  std::string lang;
  VariantMap languages = SaveAdapter::load( GameSettings::rcpath( GameSettings::langModel ) );
  for( VariantMap::iterator it=languages.begin(); it != languages.end(); it++ )
  {
    if( item.getText() == it->first )
    {
      lang = it->second.toString();
      break;
    }
  }

  GameSettings::set( GameSettings::language, Variant( std::string( lang ) ) );

  Locale::setLanguage( GameSettings::get( GameSettings::language ).toString() );
}

void ScreenMenu::Impl::resolveChangePlayerName()
{
  gui::WindowPlayerName* dlg = new gui::WindowPlayerName( game->getGui()->getRootWidget() );

  CONNECT( dlg, onNameChange(), this, Impl::setPlayerName );
  CONNECT( dlg, onClose(), this, Impl::resolveNewGame );
}

void ScreenMenu::Impl::resolveNewGame()
{
  result=startNewGame; isStopped=true;
}

void ScreenMenu::Impl::resolvePlayMission()
{
  gui::Widget* parent = game->getGui()->getRootWidget();
  Size rootSize = parent->getSize();
  Size windowSize( 512, 384 );
  Rect rect( Point( (rootSize - windowSize).getWidth() / 2, ( rootSize - windowSize ).getHeight() / 2),
             windowSize );

  gui::LoadMapWindow* wnd = new gui::LoadMapWindow( parent, rect,
                                                    GameSettings::rcpath( "/missions/" ), ".mission", -1 );

  CONNECT( wnd, onSelectFile(), this, Impl::resolveSelectFile );
  wnd->setTitle( _("##mainmenu_playmission##") );
}

void ScreenMenu::Impl::resolveShowLoadMapWnd()
{
  gui::Widget* parent = game->getGui()->getRootWidget();
  Size rootSize = parent->getSize();
  Size windowSize( 512, 384 );
  Rect rect( Point( (rootSize - windowSize).getWidth() / 2, ( rootSize - windowSize ).getHeight() / 2),
             windowSize );

  gui::LoadMapWindow* wnd = new gui::LoadMapWindow( parent,
                                                    rect,
                                                    GameSettings::rcpath( "/maps/" ), ".map",
                                                    -1 );

  CONNECT( wnd, onSelectFile(), this, Impl::resolveSelectFile );
  wnd->setTitle( _("##mainmenu_loadmap##") );
}

ScreenMenu::ScreenMenu( Game& game, GfxEngine& engine ) : _d( new Impl )
{
  _d->bgPicture = Picture::getInvalid();
  _d->isStopped = false;
  _d->game = &game;
  _d->engine = &engine;
}

ScreenMenu::~ScreenMenu() {}

void ScreenMenu::draw()
{
  _d->game->getGui()->beforeDraw();

  _d->engine->drawPicture(_d->bgPicture, 0, 0);

  _d->game->getGui()->draw();
}

void ScreenMenu::handleEvent( NEvent& event )
{
  _d->game->getGui()->handleEvent( event );
}

void ScreenMenu::initialize()
{
  _d->bgPicture = Picture::load("title", 1);

  // center the bgPicture on the screen
  Point p( (_d->engine->getScreenWidth() - _d->bgPicture.getWidth()) / 2,
           -( _d->engine->getScreenHeight() - _d->bgPicture.getHeight() ) / 2 );
  _d->bgPicture.setOffset( p );

  _d->game->getGui()->clear();

  _d->menu = new gui::StartMenu( _d->game->getGui()->getRootWidget() );

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

  btn = _d->menu->addButton( _("##mainmenu_quit##"), -1 );
  CONNECT( btn, onClicked(), _d.data(), Impl::resolveQuitGame );
}

int ScreenMenu::getResult() const
{
  return _d->result;
}

bool ScreenMenu::isStopped() const
{
  return _d->isStopped;
}

const std::string& ScreenMenu::getMapName() const
{
  return _d->fileMap;
}
