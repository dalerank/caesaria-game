// This file is part of openCaesar3.
//
// openCaesar3 is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// openCaesar3 is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with openCaesar3.  If not, see <http://www.gnu.org/licenses/>.
//
// Copyright 2012-2013 Gregoire Athanase, gathanase@gmail.com


#include "oc3_gettext.hpp"
#include "oc3_screen_menu.hpp"

#include "oc3_loadmapwindow.hpp"
#include "oc3_gfx_engine.hpp"
#include "oc3_exception.hpp"
#include "oc3_startmenu.hpp"
#include "oc3_guienv.hpp"
#include "oc3_pushbutton.hpp"
#include "oc3_gui_label.hpp"
#include "oc3_game_settings.hpp"

class ScreenMenu::Impl
{
public:
  Picture bgPicture;
  StartMenu* menu;         // menu to display
  int result;
  bool isStopped;
  std::string fileMap;

  void resolveNewGame() { result=startNewGame; isStopped=true; }
  void resolveLoadGame( std::string fileName ) 
  {
    result=loadSavedGame; 
    fileMap = fileName;
    isStopped=true; 
  }
  
  void resolvePlayMission();
  void resolveQuitGame() { result=closeApplication; isStopped=true; }
  
  void resolveSelectFile( std::string fileName )
  {
    result = loadMap;
    fileMap = fileName;
    isStopped = true;
  }

  void resolveShowLoadMapWnd();
  void resolveShowLoadGameWnd();
};

void ScreenMenu::Impl::resolveShowLoadGameWnd()
{
  Widget* parent = GuiEnv::instance().getRootWidget();
  Size rootSize = parent->getSize();
  RectF rect( 0.25f * rootSize.getWidth(), 0.25f * rootSize.getHeight(), 
              0.75f * rootSize.getWidth(), 0.75f * rootSize.getHeight() );

  io::FilePath savesPath = io::FileDir::getApplicationDir().addEndSlash().toString() + "saves/";

  LoadMapWindow* wnd = new LoadMapWindow( parent, rect.toRect(), savesPath, ".oc3save",-1 );

  CONNECT( wnd, onSelectFile(), this, Impl::resolveSelectFile );
  wnd->setTitle( _("##Load save##") );
}

void ScreenMenu::Impl::resolvePlayMission()
{
  Widget* parent = GuiEnv::instance().getRootWidget();
  Size rootSize = parent->getSize();
  RectF rect( 0.25f * rootSize.getWidth(), 0.25f * rootSize.getHeight(), 
    0.75f * rootSize.getWidth(), 0.75f * rootSize.getHeight() );
  LoadMapWindow* wnd = new LoadMapWindow( parent, rect.toRect(),
                                          GameSettings::rcpath( "/missions/" ), ".oc3mission", -1 );

  CONNECT( wnd, onSelectFile(), this, Impl::resolveSelectFile );
  wnd->setTitle( _("##Select mission##") );
}

void ScreenMenu::Impl::resolveShowLoadMapWnd()
{
  Widget* parent = GuiEnv::instance().getRootWidget();
  Size rootSize = parent->getSize();
  LoadMapWindow* wnd = new LoadMapWindow( parent,
                                          RectF( 0.25f * rootSize.getWidth(), 0.25f * rootSize.getHeight(), 
                                                0.75f * rootSize.getWidth(), 0.75f * rootSize.getHeight() ).toRect(), 
                                                GameSettings::rcpath( "/maps/" ), ".map",
                                                -1 );

  CONNECT( wnd, onSelectFile(), this, Impl::resolveSelectFile );
  wnd->setTitle( _("##Load map##") );
}

ScreenMenu::ScreenMenu() : _d( new Impl )
{
  _d->bgPicture = Picture::getInvalid();
  _d->isStopped = false;
}

ScreenMenu::~ScreenMenu() {}

void ScreenMenu::draw()
{
  GuiEnv::instance().beforeDraw();

  GfxEngine::instance().drawPicture(_d->bgPicture, 0, 0);

  GuiEnv::instance().draw();
}

void ScreenMenu::handleEvent( NEvent& event )
{
  GuiEnv::instance().handleEvent( event );
}

void ScreenMenu::initialize()
{
  _d->bgPicture = Picture::load("title", 1);

  // center the bgPicture on the screen
  GfxEngine& engine = GfxEngine::instance();

  _d->bgPicture.setOffset( (engine.getScreenWidth() - _d->bgPicture.getWidth()) / 2,
                             -( engine.getScreenHeight() - _d->bgPicture.getHeight() ) / 2 );

  GuiEnv::instance().clear();

  _d->menu = new StartMenu( GuiEnv::instance().getRootWidget() );

  PushButton* btn = _d->menu->addButton( _("##mainmenu_newgame##"), -1 );
  CONNECT( btn, onClicked(), _d.data(), Impl::resolveNewGame );

  btn = _d->menu->addButton( _("##mainmenu_playmission##"), -1 );
  CONNECT( btn, onClicked(), _d.data(), Impl::resolvePlayMission );

  btn = _d->menu->addButton( _("##mainmenu_loadgame##"), -1 );
  CONNECT( btn, onClicked(), _d.data(), Impl::resolveShowLoadGameWnd );

  btn = _d->menu->addButton( _("##mainmenu_loadmap##"), -1 );
  CONNECT( btn, onClicked(), _d.data(), Impl::resolveShowLoadMapWnd );

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
