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

#include "oc3_screen_game.hpp"

#include <algorithm>
#include <iostream>

#include "oc3_gfx_engine.hpp"
#include "oc3_gfx_sdl_engine.hpp"
#include "oc3_exception.hpp"
#include "oc3_warehouse.hpp"
#include "oc3_scenario_saver.hpp"
#include "oc3_menurgihtpanel.hpp"
#include "oc3_resourcegroup.hpp"
#include "oc3_guienv.hpp"
#include "oc3_topmenu.hpp"
#include "oc3_menu.hpp"
#include "oc3_event.hpp"
#include "oc3_infoboxmanager.hpp"
#include "oc3_constructionmanager.hpp"
#include "oc3_tilemapchangecommand.hpp"

class ScreenGame::Impl
{
public:
  MenuRigthPanel* rightPanel;
  GuiEnv* gui;
  GfxEngine* engine;
  TopMenu* topMenu;
  Menu* menu;
  ExtentMenu* extMenu;
  InfoBoxManagerPtr infoBoxMgr;
  GuiTilemap guiTilemap;
  Scenario* scenario; // current game scenario
  
  int result;
};

ScreenGame::ScreenGame() : _d( new Impl )
{
  _d->topMenu = NULL;
  _d->scenario = NULL;
}

ScreenGame::~ScreenGame() {}

void ScreenGame::initialize( GfxEngine& engine, GuiEnv& gui )
{
  _d->gui = &gui;
  _d->engine = &engine;
  _d->infoBoxMgr = InfoBoxManager::create( &gui );
  // enable key repeat, 1ms delay, 100ms repeat
  SDL_EnableKeyRepeat(1, 100);

  _d->gui->clear();

  const int topMenuHeight = 23;
  const Picture& rPanelPic = PicLoader::instance().get_picture( ResourceGroup::panelBackground, 14 );
  Rect rPanelRect( engine.getScreenWidth() - rPanelPic.get_width(), topMenuHeight,
                   engine.getScreenWidth(), engine.getScreenHeight() );
  _d->rightPanel = MenuRigthPanel::create( gui.getRootWidget(), rPanelRect, rPanelPic);

  _d->topMenu = TopMenu::create( gui.getRootWidget(), topMenuHeight );

  _d->menu = Menu::create( gui.getRootWidget(), -1 );
  _d->menu->setPosition( Point( engine.getScreenWidth() - _d->menu->getWidth() - _d->rightPanel->getWidth(), 
                                 _d->topMenu->getHeight() ) );

  _d->extMenu = ExtentMenu::create( gui.getRootWidget(), _d->guiTilemap, -1 );
  _d->extMenu->setPosition( Point( engine.getScreenWidth() - _d->extMenu->getWidth() - _d->rightPanel->getWidth(), 
                                     _d->topMenu->getHeight() ) );
    
  _d->rightPanel->bringToFront();

  // 8*30: used for high buildings (granary...), visible even when not in tilemap_area.
  getMapArea().setViewSize( engine.getScreenWidth(), engine.getScreenHeight() + 8 * 30);
        
  // here move camera to start position of map
  getMapArea().setCenterIJ( _d->scenario->getCity().getCameraPos() ); 

  //connect elements
  CONNECT( _d->topMenu, onSave(), this, ScreenGame::resolveGameSave );
  CONNECT( _d->topMenu, onExit(), this, ScreenGame::resolveExitGame );
  CONNECT( _d->topMenu, onEnd(), this, ScreenGame::resolveEndGame );

  CONNECT( _d->menu, onCreateConstruction(), this, ScreenGame::resolveCreateConstruction );
  CONNECT( _d->menu, onRemoveTool(), this, ScreenGame::resolveRemoveTool );
  CONNECT( _d->menu, onMaximize(), _d->extMenu, ExtentMenu::maximize );

  CONNECT( _d->extMenu, onCreateConstruction(), this, ScreenGame::resolveCreateConstruction );
  CONNECT( _d->extMenu, onRemoveTool(), this, ScreenGame::resolveRemoveTool );

  CONNECT( &_d->scenario->getCity(), onPopulationChanged(), _d->topMenu, TopMenu::setPopulation );
  CONNECT( &_d->scenario->getCity(), onFundsChanged(), _d->topMenu, TopMenu::setFunds );
  CONNECT( &_d->scenario->getCity(), onMonthChanged(), _d->topMenu, TopMenu::setDate );

  CONNECT( &_d->guiTilemap, onShowTileInfo(), this, ScreenGame::showTileInfo );
}

void ScreenGame::resolveGameSave()
{
  ScenarioSaver scnSaver( Scenario::instance() );

  scnSaver.save( "./test.oc3save" );
}

TilemapArea& ScreenGame::getMapArea()
{
  return _mapArea;
}

void ScreenGame::setScenario(Scenario &scenario)
{
  _d->scenario = &scenario;
  City& city = scenario.getCity();
  Tilemap& tilemap = city.getTilemap();

  _mapArea.init(tilemap);
  _d->guiTilemap.init(city, _mapArea, this);
}

void ScreenGame::drawTilemap()
{
  _d->guiTilemap.drawTilemap();
}

void ScreenGame::drawInterface()
{
  _d->gui->beforeDraw();
  _d->gui->draw();
}

void ScreenGame::draw()
{
  drawTilemap();
  drawInterface();
}

void ScreenGame::afterFrame()
{
  _d->scenario->getCity().timeStep();
}

void ScreenGame::handleEvent( NEvent& event )
{
  //After MouseDown events are send to the same target till MouseUp
  static enum _MouseEventTarget
  {
    _MET_NONE,
    _MET_GUI,
    _MET_TILES
  } _mouseEventTarget = _MET_NONE;

  bool eventResolved = false;
  if (event.EventType == OC3_MOUSE_EVENT)
  {
    if (event.EventType & (OC3_RMOUSE_PRESSED_DOWN | OC3_LMOUSE_PRESSED_DOWN))
    {
      eventResolved = _d->gui->handleEvent( event );
      if (eventResolved)
      {
        _mouseEventTarget = _MET_GUI;
      }
      else // eventresolved
      {
        _mouseEventTarget = _MET_TILES;
        _d->guiTilemap.handleEvent( event );
      }
      return;
    }

    switch(_mouseEventTarget)
    {
    case _MET_GUI:
      _d->gui->handleEvent( event );
      break;
    case _MET_TILES:
      _d->guiTilemap.handleEvent( event );
      break;
    default:
      if (!_d->gui->handleEvent( event ))
        _d->guiTilemap.handleEvent( event );
      break;
    }

    if (event.EventType & (OC3_RMOUSE_LEFT_UP | OC3_LMOUSE_LEFT_UP))
      _mouseEventTarget = _MET_NONE;
  }
  else
  {
    eventResolved = _d->gui->handleEvent( event );      
   
    if( !eventResolved )
      _d->guiTilemap.handleEvent( event );

    if( event.EventType == OC3_KEYBOARD_EVENT )
    {
      switch( event.KeyboardEvent.Key )
      {
	case KEY_ESCAPE:
          stop();
	  break;
	case KEY_F10:
	  makeScreenShot();
	  break;
      }
    }
  }
}

void ScreenGame::makeScreenShot()
{
  std::cout << "creating screenshot" << std::endl;
  // get date
  std::string filename ("screenshot.bmp");
  // write file
  SDL_Surface* surface = dynamic_cast<GfxSdlEngine*>(_d->engine)->getScreen().get_surface();
  SDL_SaveBMP(surface, filename.c_str());
}

int ScreenGame::getResult() const
{
  return _d->result;
}

void ScreenGame::resolveCreateConstruction( int type )
{
  _d->guiTilemap.setChangeCommand( TilemapChangeCommand( BuildingType( type ) ) );
}

void ScreenGame::resolveRemoveTool()
{
  _d->guiTilemap.setChangeCommand( TilemapRemoveCommand() );
}

void ScreenGame::showTileInfo( Tile* tile )
{
  if( tile )
    _d->infoBoxMgr->showHelp( tile );
}

void ScreenGame::resolveEndGame()
{
  _d->result = ScreenGame::mainMenu;
  stop();
}

void ScreenGame::resolveExitGame()
{
  _d->result = ScreenGame::quitGame;
  stop();
}
