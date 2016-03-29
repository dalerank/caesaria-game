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

#include "androidactions.hpp"
#include "environment.hpp"
#include "texturedbutton.hpp"
#include "scene/level.hpp"
#include "gfx/camera.hpp"
#include "widget_helper.hpp"
#include "events/showtileinfo.hpp"
#include "events/script_event.hpp"
#include "core/logger.hpp"
#include "gfx/tile.hpp"
#include "gui/ingame_menu.hpp"
#include <GameVfs>
#include <GameCore>

using namespace gfx;
using namespace events;

namespace gui
{

namespace tablet
{

class MouseEventHandler : scene::EventHandler
{
public:
  Camera* camera;
  Signal1<TilePos> onTileChange;
  Rect forbidenArea;

  void install( scene::Base* scene, const Rect& forbiden )
  {
    forbidenArea = forbiden;
    scene->installEventHandler( this );
    camera = scene->camera();
    drop();
  }

  virtual void handleEvent( NEvent& event )
  {
    if( camera == nullptr )
      return;

    if( event.EventType == sEventMouse  )
    {
      if( event.mouse.type == NEvent::Mouse::mouseLbtnRelease )
      {
        if( forbidenArea.isPointInside( event.mouse.pos() ) )
          return;

        Tile* lastTile = camera->at( event.mouse.pos(), false );
        if( lastTile != nullptr )
          emit onTileChange( lastTile->epos() );
      }
    }
  }

  virtual bool finished() const { return false; }
};

class ActionsBar::Impl
{
public:
  TexturedButton* btnShowHelp;
  TexturedButton* btnExit;
  TexturedButton* btnMenu;
  TexturedButton* btnEnter;
  TexturedButton* btnZoomIn;
  TexturedButton* btnZoomOut;

public signals:
  Signal1<int> onChangeZoomSignal;
};

ActionsBar::ActionsBar( Widget* parent)
  : Window( parent, Rect( 0, 0, 1, 1 ), "", Hash(TEXT(AndroidActionsBar)), bgNone ), _d( new Impl )
{
  setupUI( ":/gui/android_actions_bar.gui" );

  setInternalName("AndroidActionsBar");
  GET_DWIDGET_FROM_UI( _d, btnMenu     )
  GET_DWIDGET_FROM_UI( _d, btnShowHelp )
  GET_DWIDGET_FROM_UI( _d, btnEnter    )
  GET_DWIDGET_FROM_UI( _d, btnExit     )
  GET_DWIDGET_FROM_UI( _d, btnZoomIn   )
  GET_DWIDGET_FROM_UI( _d, btnZoomOut  )
}

Signal0<>& ActionsBar::onRequestTileHelp() { return _d->btnShowHelp->onClicked(); }
Signal0<>& ActionsBar::onEscapeClicked()   { return _d->btnExit->onClicked(); }
Signal0<>& ActionsBar::onEnterClicked()    { return _d->btnEnter->onClicked(); }
Signal0<>& ActionsBar::onRequestMenu()     { return _d->btnMenu->onClicked(); }
Signal1<int>& ActionsBar::onChangeZoom()     { return _d->onChangeZoomSignal; }

void ActionsBar::beforeDraw(gfx::Engine& painter)
{
  if( !visible() )
    return;

  if( parent()->children().back() != this )
  {
    bringToFront();
  }

  Window::beforeDraw( painter );
}

bool ActionsBar::onEvent(const NEvent &event)
{
  if( event.EventType == sEventGui && event.gui.type == guiButtonClicked )
  {
    if( event.gui.caller == _d->btnZoomIn || event.gui.caller == _d->btnZoomOut )
    {
      emit _d->onChangeZoomSignal( event.gui.caller == _d->btnZoomIn ? +10 : -10 );
      return true;
    }
  }
  else if( event.EventType == sEventMouse && event.mouse.type == NEvent::Mouse::moved )
  {
    return true;
  }

  return Window::onEvent( event );
}


void ActionsHandler::assignTo(ActionsBar* parent, scene::Base* scene )
{
  ActionsBar* androidBar = safety_cast<ActionsBar*>( parent );
  if( androidBar && scene )
  {
    ActionsHandler* handler = new ActionsHandler( androidBar, scene );
    auto eventHandler = new MouseEventHandler();
    eventHandler->install( scene, androidBar->absoluteRect() );

    CONNECT( eventHandler, onTileChange,      handler,         ActionsHandler::_setActiveTile )
    CONNECT( androidBar, onRequestTileHelp(), handler,         ActionsHandler::_showTileHelp )
    CONNECT( androidBar, onEscapeClicked(),   handler,         ActionsHandler::_resolveEscapeButton )
    CONNECT( androidBar, onEnterClicked(),    handler,         ActionsHandler::_resolveEnterButton  )
    CONNECT( androidBar, onRequestMenu(),     handler,         ActionsHandler::_showIngameMenu      )
    CONNECT( androidBar, onChangeZoom(),      scene->camera(), gfx::Camera::changeZoom )
  }
}

ActionsHandler::ActionsHandler(Widget* parent, scene::Base* scene)
  : Widget( parent, -1, Rect() )
{
   _scene = scene;
}

void ActionsHandler::_setActiveTile(TilePos tilepos) {  _tilepos = tilepos; }
void ActionsHandler::_resolveEnterButton() { _sendKeyboardEvent( KEY_RETURN ); }
void ActionsHandler::_resolveEscapeButton() { _sendKeyboardEvent( KEY_ESCAPE ); }

void ActionsHandler::_resolveExitGame()
{
  if( !_scene )
    return;

  NEvent e;
  e.EventType = sEventQuit;

  _scene->handleEvent( e );
}

void ActionsHandler::_sendKeyboardEvent(int key, bool ctrl)
{
  if( !_scene )
    return;

  NEvent e;

  e.EventType = sEventKeyboard;
  e.keyboard.key = (KeyCode)key;
  e.keyboard.pressed = false;
  e.keyboard.shift = false;
  e.keyboard.control = ctrl;
  e.keyboard.symbol = 0;

  _scene->handleEvent( e );
}

void ActionsHandler::_showIngameMenu()
{
  IngameMenu* menu = &ui()->add<IngameMenu>();
  if( menu )
  {
    CONNECT( menu, onExit(),    this, ActionsHandler::_resolveExitGame )
    CONNECT( menu, onLoad(),    this, ActionsHandler::_showLoadDialog  )
    CONNECT( menu, onSave(),    this, ActionsHandler::_showSaveDialog  )
    CONNECT( menu, onRestart(), this, ActionsHandler::_restartGame     )
    CONNECT( menu, onMenu(),    this, ActionsHandler::_exitToMainMenu  )
  }
}

void ActionsHandler::_showSaveDialog() { events::dispatch<ScriptFunc>("OnShowSaveDialog"); }
void ActionsHandler::_showLoadDialog() { events::dispatch<ScriptFunc>("OnShowSaveSelectDialog"); }

void ActionsHandler::_restartGame()
{
  if( _scene )
    _scene->setMode(scene::Level::res_restart);
}

void ActionsHandler::_exitToMainMenu()
{
  if( _scene )
    _scene->setMode(scene::Level::res_menu);
}

void ActionsHandler::_showTileHelp()
{
  if( !_scene )
    return;

  events::dispatch<ShowTileInfo>( _tilepos );
}

}//end namespace tablet

}//end namespace gui-
