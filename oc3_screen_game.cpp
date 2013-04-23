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
};

ScreenGame::ScreenGame() : _d( new Impl )
{
   _d->topMenu = NULL;
   _scenario = NULL;
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

    _d->extMenu = ExtentMenu::create( gui.getRootWidget(), -1 );
    _d->extMenu->setPosition( Point( engine.getScreenWidth() - _d->extMenu->getWidth() - _d->rightPanel->getWidth(), 
                                     _d->topMenu->getHeight() ) );

    //over other elements
    _d->rightPanel->bringToFront();

    CONNECT( _d->menu, onCreateConstruction(), this, ScreenGame::resolveCreateConstruction );
    CONNECT( _d->menu, onRemoveTool(), this, ScreenGame::resolveRemoveTool );
    CONNECT( _d->menu, onMaximize(), _d->extMenu, ExtentMenu::maximize );

    CONNECT( _d->extMenu, onCreateConstruction(), this, ScreenGame::resolveCreateConstruction );
    CONNECT( _d->extMenu, onRemoveTool(), this, ScreenGame::resolveRemoveTool );

    CONNECT( &_scenario->getCity(), onPopulationChanged(), _d->topMenu, TopMenu::setPopulation );
    CONNECT( &_scenario->getCity(), onFundsChanged(), _d->topMenu, TopMenu::setFunds );
    CONNECT( &_scenario->getCity(), onMonthChanged(), _d->topMenu, TopMenu::setDate );

    CONNECT( &_guiTilemap, onShowTileInfo(), this, ScreenGame::showTileInfo );
  /* _d->extMenu = ExtentMenu::create();
   _d->extMenu->setPosition( engine.getScreenWidth() - _d->extMenu->getWidth() - _d->rightPanel->getWidth(), 
                             _d->topMenu->getHeight() ); */


    getMapArea().setViewSize( engine.getScreenWidth(), 
                             engine.getScreenHeight() + 8 * 30);  // 8*30: used for high buildings (granary...), visible even when not in tilemap_area.
    
    getMapArea().setCenterIJ( _scenario->getCity().getCameraStartIJ() ); // here move camera to start position of map
}

TilemapArea& ScreenGame::getMapArea()
{
   return _mapArea;
}

void ScreenGame::setScenario(Scenario &scenario)
{
   _scenario = &scenario;
   City& city = scenario.getCity();
   Tilemap& tilemap = city.getTilemap();

   _mapArea.init(tilemap);
   _guiTilemap.init(city, _mapArea, this);
}

void ScreenGame::drawTilemap()
{
  _guiTilemap.drawTilemap();
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
   _scenario->getCity().timeStep();
}

void ScreenGame::handleEvent( NEvent& event )
{
    bool eventResolved = _d->gui->handleEvent( event );      
   
    if( !eventResolved )
        _guiTilemap.handleEvent( event );

    if( event.EventType == OC3_KEYBOARD_EVENT && event.KeyboardEvent.Key == KEY_ESCAPE )
    {
        std::cout << "EVENT_ESCAPE was pressed" << std::endl;
        stop();
    }
}

int ScreenGame::getResult() const
{
	return 0;
}

void ScreenGame::resolveCreateConstruction( int type )
{
    _guiTilemap.setChangeCommand( TilemapChangeCommand( BuildingType( type ) ) );
}

void ScreenGame::resolveRemoveTool()
{
    _guiTilemap.setChangeCommand( TilemapRemoveCommand() );
}

void ScreenGame::showTileInfo( Tile* tile )
{
    _d->infoBoxMgr->showHelp( tile );
}
// void ScreenGame::handleWidgetEvent(const WidgetEvent& event, Widget *widget)
// {
//    
//    else if (event._eventType == WE_InGameMenu)
//    {
//       GfxEngine &engine = GfxEngine::instance();
//       InGameMenu* inGameMenu = new InGameMenu();
//       inGameMenu->init();
//       inGameMenu->setPosition(engine.getScreenWidth() - inGameMenu->getWidth() - _menu->getWidth()-5, 50);
//       setInGameMenu(inGameMenu);
//    }
//    else if (event._eventType == WE_SaveGame)
//    {
//       if (_scenario != NULL)
//       {
//          std::cout << "SAVE" << std::endl;
//          ScenarioSaver saver = ScenarioSaver();
//          saver.save("oc3.sav");
//          if (_inGameMenu != NULL)
//          {
//             // delete menu, if any
//             _inGameMenu->setDeleted();
//          }
//       }
//    }
//  
// 
// }
