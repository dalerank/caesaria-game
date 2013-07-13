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

#include "oc3_gfx_engine.hpp"
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
#include "oc3_message_stack_widget.hpp"
#include "oc3_time.hpp"
#include "oc3_stringhelper.hpp"
#include "oc3_empiremap_window.hpp"
#include "oc3_save_dialog.hpp"
#include "oc3_advisors_window.hpp"
#include "oc3_alarm_event_holder.hpp"
#include "oc3_tilemap_renderer.hpp"
#include "oc3_scenario.hpp"
#include "oc3_senate_popup_info.hpp"
#include "oc3_cityfunds.hpp"

class ScreenGame::Impl
{
public:
  MenuRigthPanel* rightPanel;
  GuiEnv* gui;
  GfxEngine* engine;
  TopMenu* topMenu;
  Menu* menu;
  GuiInfoBox* infoBox;   // info box to display, if any
  TilemapArea mapArea;  // visible map area
  ExtentMenu* extMenu;
  InfoBoxManagerPtr infoBoxMgr;
  TilemapRenderer mapRenderer;
  WindowMessageStack* wndStackMsgs;
  Scenario* scenario; // current game scenario
  AlarmEventHolder alarmsHolder;

  int result;

  void resolveGameSave( std::string filename );
  void showSaveDialog();
  void showEmpireMapWindow();
  void showAdvisorsWindow( const int advType );
  void showAdvisorsWindow();
  void showTradeAdvisorWindow();
  void resolveCreateConstruction( int type );
  void resolveSelectOverlayView( int type );
  void resolveRemoveTool();
  void showTileInfo( const Tile& tile );
  void makeScreenShot();
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

  CityPtr city = _d->scenario->getCity();
  // enable key repeat, 1ms delay, 100ms repeat

  _d->gui->clear();

  /*new PopupMessageBox( _d->gui->getRootWidget(), "Test title", "This is test string for popup message box", 
                       "Sen 351 BC", "For New player" ); */

  const int topMenuHeight = 23;
  const Picture& rPanelPic = Picture::load( ResourceGroup::panelBackground, 14 );
  Rect rPanelRect( engine.getScreenWidth() - rPanelPic.getWidth(), topMenuHeight,
                   engine.getScreenWidth(), engine.getScreenHeight() );

  _d->rightPanel = MenuRigthPanel::create( gui.getRootWidget(), rPanelRect, rPanelPic);

  _d->topMenu = TopMenu::create( gui.getRootWidget(), topMenuHeight );
  _d->topMenu->setPopulation( city->getPopulation() );
  _d->topMenu->setFunds( city->getFunds().getValue() );
  _d->topMenu->setDate( city->getDate() );

  _d->menu = Menu::create( gui.getRootWidget(), -1, _d->scenario->getCity() );
  _d->menu->setPosition( Point( engine.getScreenWidth() - _d->menu->getWidth() - _d->rightPanel->getWidth(), 
                                 _d->topMenu->getHeight() ) );

  _d->extMenu = ExtentMenu::create( gui.getRootWidget(), _d->mapRenderer, -1, _d->scenario->getCity() );
  _d->extMenu->setPosition( Point( engine.getScreenWidth() - _d->extMenu->getWidth() - _d->rightPanel->getWidth(), 
                                     _d->topMenu->getHeight() ) );

  _d->wndStackMsgs = WindowMessageStack::create( gui.getRootWidget(), -1 );
  _d->wndStackMsgs->setPosition( Point( gui.getRootWidget()->getWidth() / 4, 33 ) );
  _d->wndStackMsgs->sendToBack();
    
  _d->rightPanel->bringToFront();

  // 8*30: used for high buildings (granary...), visible even when not in tilemap_area.
  getMapArea().setViewSize( engine.getScreenSize() + Size( 180 ) );
        
  // here move camera to start position of map
  getMapArea().setCenterIJ( _d->scenario->getCity()->getCameraPos() ); 

  new SenatePopupInfo( _d->gui->getRootWidget(), _d->mapRenderer );

  //connect elements
  CONNECT( _d->topMenu, onSave(), _d.data(), Impl::showSaveDialog );
  CONNECT( _d->topMenu, onExit(), this, ScreenGame::resolveExitGame );
  CONNECT( _d->topMenu, onEnd(), this, ScreenGame::resolveEndGame );
  CONNECT( _d->topMenu, onRequestAdvisor(), _d.data(), Impl::showAdvisorsWindow );

  CONNECT( _d->menu, onCreateConstruction(), _d.data(), Impl::resolveCreateConstruction );
  CONNECT( _d->menu, onRemoveTool(), _d.data(), Impl::resolveRemoveTool );
  CONNECT( _d->menu, onMaximize(), _d->extMenu, ExtentMenu::maximize );

  CONNECT( _d->extMenu, onCreateConstruction(), _d.data(), Impl::resolveCreateConstruction );
  CONNECT( _d->extMenu, onRemoveTool(), _d.data(), Impl::resolveRemoveTool );

  CONNECT( city, onPopulationChanged(), _d->topMenu, TopMenu::setPopulation );
  CONNECT( city, onFundsChanged(), _d->topMenu, TopMenu::setFunds );
  CONNECT( city, onMonthChanged(), _d->topMenu, TopMenu::setDate );

  CONNECT( &_d->mapRenderer, onShowTileInfo(), _d.data(), Impl::showTileInfo );

  CONNECT( city, onWarningMessage(), _d->wndStackMsgs, WindowMessageStack::addMessage );
  CONNECT( &_d->mapRenderer, onWarningMessage(), _d->wndStackMsgs, WindowMessageStack::addMessage );
  CONNECT( _d->extMenu, onSelectOverlayType(), _d.data(), Impl::resolveSelectOverlayView );
  CONNECT( _d->extMenu, onEmpireMapShow(), _d.data(), Impl::showEmpireMapWindow );
  CONNECT( _d->extMenu, onAdvisorsWindowShow(), _d.data(), Impl::showAdvisorsWindow );

  CONNECT( city, onDisasterEvent(), &_d->alarmsHolder, AlarmEventHolder::add );
  CONNECT( _d->extMenu, onSwitchAlarm(), &_d->alarmsHolder, AlarmEventHolder::next );
  CONNECT( &_d->alarmsHolder, onMoveToAlarm(), &_d->mapArea, TilemapArea::setCenterIJ );
  CONNECT( &_d->alarmsHolder, onAlarmChange(), _d->extMenu, ExtentMenu::setAlarmEnabled );
}

void ScreenGame::Impl::showSaveDialog()
{
  SaveDialog* dialog = new SaveDialog( gui->getRootWidget(), "./saves/", ".oc3save", -1 );
  CONNECT( dialog, onFileSelected(), this, Impl::resolveGameSave );
}

void ScreenGame::Impl::resolveGameSave( std::string filename )
{
  ScenarioSaver scnSaver( Scenario::instance() );

  scnSaver.save( filename );
}

TilemapArea& ScreenGame::getMapArea()
{
  return _d->mapArea;
}

void ScreenGame::setScenario(Scenario& scenario)
{
  _d->scenario = &scenario;
  CityPtr city = scenario.getCity();
  Tilemap& tilemap = city->getTilemap();

  _d->mapArea.init( tilemap );
  _d->mapRenderer.init( city, _d->mapArea, this);
}

void ScreenGame::draw()
{
  _d->mapRenderer.drawTilemap();
  _d->gui->beforeDraw();
  _d->gui->draw();
}

void ScreenGame::afterFrame()
{
  _d->scenario->getCity()->timeStep();
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
        _d->mapRenderer.handleEvent( event );
      }
      return;
    }

    switch(_mouseEventTarget)
    {
    case _MET_GUI:
      _d->gui->handleEvent( event );
      break;
    case _MET_TILES:
      _d->mapRenderer.handleEvent( event );
      break;
    default:
      if (!_d->gui->handleEvent( event ))
        _d->mapRenderer.handleEvent( event );
      break;
    }

    if (event.EventType & (OC3_RMOUSE_LEFT_UP | OC3_LMOUSE_LEFT_UP))
      _mouseEventTarget = _MET_NONE;
  }
  else
  {
    eventResolved = _d->gui->handleEvent( event );      
   
    if( !eventResolved )
      _d->mapRenderer.handleEvent( event );

    if( event.EventType == OC3_KEYBOARD_EVENT )
    {
      switch( event.KeyboardEvent.Key )
      {
	    case KEY_ESCAPE:
       // stop();
	    break;
	    
      case KEY_F10:
	      _d->makeScreenShot();
	    break;

      default: break;
      }
    }
  }
}

void ScreenGame::Impl::makeScreenShot()
{
  DateTime time = DateTime::getCurrenTime();

  std::string filename = StringHelper::format( 0xff, "oc3_[%04d_%02d_%02d_%02d_%02d_%02d].png", 
                                               time.getYear(), time.getMonth(), time.getDay(),
                                               time.getHour(), time.getMinutes(), time.getSeconds() );
  StringHelper::debug( 0xff, "creating screenshot %s", filename.c_str() );

  engine->createScreenshot( filename );
}

int ScreenGame::getResult() const
{
  return _d->result;
}

void ScreenGame::Impl::resolveCreateConstruction( int type )
{
  mapRenderer.setMode( TilemapBuildCommand::create( BuildingType( type ) ) );
}

void ScreenGame::Impl::resolveRemoveTool()
{
  mapRenderer.setMode( TilemapRemoveCommand::create() );
}

void ScreenGame::Impl::showTileInfo( const Tile& tile )
{
  infoBoxMgr->showHelp( tile );
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

void ScreenGame::Impl::resolveSelectOverlayView( int type )
{
  mapRenderer.setMode( TilemapOverlayCommand::create( OverlayType( type ) ) );
}

void ScreenGame::Impl::showAdvisorsWindow()
{
  showAdvisorsWindow( ADV_EMPLOYERS );
}

void ScreenGame::Impl::showAdvisorsWindow( const int advType )
{  
  List<AdvisorsWindow*> wndList = gui->getRootWidget()->findChildren<AdvisorsWindow*>();

  if( wndList.size() == 1 )
  {
    wndList.front()->bringToFront();
    wndList.front()->showAdvisor( (AdvisorType)advType ); 
  }
  else
  {
    AdvisorsWindow* advWnd = AdvisorsWindow::create( gui->getRootWidget(), -1, 
                                                     (AdvisorType)advType, scenario->getCity() );
    CONNECT( advWnd, onEmpireMapRequest(), this, Impl::showEmpireMapWindow ); 
  }
}

void ScreenGame::Impl::showTradeAdvisorWindow()
{
  showAdvisorsWindow( ADV_TRADING );
}

void ScreenGame::Impl::showEmpireMapWindow()
{  
  List<EmpireMapWindow*> wndList = gui->getRootWidget()->findChildren<EmpireMapWindow*>();

  if( wndList.size() == 1 )
  {
    wndList.front()->bringToFront();
  }
  else
  {
    EmpireMapWindow* emap = EmpireMapWindow::create( gui->getRootWidget(), -1 );
    CONNECT( emap, onTradeAdvisorRequest(), this, Impl::showTradeAdvisorWindow ); 
  }  
}
