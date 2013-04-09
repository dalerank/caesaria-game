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


#include <screen_game.hpp>

#include <algorithm>
#include <iostream>

#include "gfx_engine.hpp"
#include "gfx_sdl_engine.hpp"
#include "exception.hpp"
#include "warehouse.hpp"
#include "scenario_saver.hpp"
#include "oc3_menurgihtpanel.h"
#include "oc3_resourcegroup.h"
#include "oc3_extentmenu.h"
#include "oc3_guienv.h"
#include "oc3_topmenu.h"
#include "oc3_menu.h"
#include "oc3_event.h"
#include "oc3_buildmenu.h"

class ScreenGame::Impl
{
public:
    MenuRigthPanelPtr rightPanel;
    ExtentMenuPtr extMenu;
    GuiEnv* gui;
    GfxEngine* engine;
    TopMenuPtr topMenu;
    MenuPtr menu;
};


ScreenGame::ScreenGame() : _d( new Impl )
{
   _scenario = NULL;
   _infoBox = NULL;
}

ScreenGame::~ScreenGame() {}

void ScreenGame::initialize( GfxEngine& engine, GuiEnv& gui )
{
    _d->gui = &gui;
    _d->engine = &engine;
    // enable key repeat, 1ms delay, 100ms repeat
    SDL_EnableKeyRepeat(1, 100);

    _d->gui->clear();

    const Picture& rPanelPic = PicLoader::instance().get_picture( ResourceGroup::panelBackground, 14 );
    Rect rPanelRect( engine.getScreenWidth() - rPanelPic.get_width(), 0,
                    engine.getScreenWidth(), engine.getScreenHeight() );
    _d->rightPanel = MenuRigthPanel::create( gui.getRootWidget(), rPanelRect, rPanelPic);

    _d->topMenu = TopMenu::create( gui.getRootWidget(), 23 );

    _d->menu = Menu::create( gui.getRootWidget(), -1 );
    _d->menu->setPosition( Point( engine.getScreenWidth() - _d->menu->getWidth() - _d->rightPanel->getWidth(), 
                                 _d->topMenu->getHeight() ) );

    CONNECT( _d->menu, onCreateBuildMenu(), this, ScreenGame::createBuildMenu );
    CONNECT( _d->menu, onCreateHouse(), this, ScreenGame::resolveHouseBuilding );
    CONNECT( _d->menu, onCreateRoad(), this, ScreenGame::resolveRoadBuilding );
    CONNECT( _d->menu, onRemoveTool(), this, ScreenGame::resolveRemoveTool );
  /* _d->extMenu = ExtentMenu::create();
   _d->extMenu->setPosition( engine.getScreenWidth() - _d->extMenu->getWidth() - _d->rightPanel->getWidth(), 
                             _d->topMenu->getHeight() ); */


    getMapArea().setViewSize( engine.getScreenWidth(), 
                             engine.getScreenHeight()+8*30);  // 8*30: used for high buildings (granary...), visible even when not in tilemap_area.
    getMapArea().setCenterIJ(25, 10);
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
        std::cout << "SDLK_ESCAPE was pressed" << std::endl;
        stop();
    }
}

int ScreenGame::getResult() const
{
	return 0;
}

void ScreenGame::createBuildMenu( int type, Widget* parent )
{
    BuildMenu* buildMenu = BuildMenu::getMenuInstance( (BuildMenuType)type, _d->gui->getRootWidget() );
    
    if( buildMenu != NULL )
    {
       // we have a new buildMenu: initialize it
       GfxEngine &engine = GfxEngine::instance();
    
       // compute the Y position of the menu, ugly because of submenus   
       buildMenu->init();
       int y = math::clamp< int >( parent->getScreenTop(), 0, engine.getScreenHeight() - buildMenu->getHeight() );
       buildMenu->setPosition( Point( engine.getScreenWidth() - buildMenu->getWidth() - _d->menu->getWidth() - 5, y ) );
    }
}

void ScreenGame::resolveHouseBuilding()
{
    Construction *construction = dynamic_cast<Construction*>(LandOverlay::getInstance(B_HOUSE));
    _guiTilemap.setBuildInstance(construction);
}

void ScreenGame::resolveRoadBuilding()
{
    Construction *construction = dynamic_cast<Construction*>(LandOverlay::getInstance(B_ROAD));
    _guiTilemap.setBuildInstance(construction);
}

void ScreenGame::resolveRemoveTool()
{
    _guiTilemap.setRemoveTool();
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
