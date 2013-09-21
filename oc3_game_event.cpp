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

#include "oc3_game_event.hpp"
#include "oc3_constructionmanager.hpp"
#include "oc3_city.hpp"
#include "oc3_gettext.hpp"
#include "oc3_building_data.hpp"
#include "oc3_cityfunds.hpp"
#include "oc3_guienv.hpp"
#include "oc3_gui_info_box.hpp"
#include "oc3_tilemap.hpp"
#include "oc3_game.hpp"
#include "oc3_game_event_mgr.hpp"
#include "oc3_stringhelper.hpp"
#include "oc3_gui_label.hpp"
#include "oc3_window_empiremap.hpp"
#include "oc3_empire.hpp"
#include "oc3_resourcegroup.hpp"
#include "oc3_advisors_window.hpp"
#include "oc3_city_trade_options.hpp"

static const int windowGamePausedId = StringHelper::hash( "gamepause" );

GameEventPtr DisasterEvent::create( const TilePos& pos, Type type )
{
  DisasterEvent* event = new DisasterEvent();
  event->_pos = pos;
  event->_type = type;

  GameEventPtr ret( event );
  ret->drop();

  return ret;
}

void DisasterEvent::exec( Game& game )
{
  Tilemap& tmap = game.getCity()->getTilemap();
  Tile& tile = tmap.at( _pos );
  TilePos rPos = _pos;

  if( tile.getFlag( Tile::isDestructible ) )
  {
    Size size( 1 );

    LandOverlayPtr overlay = tile.getOverlay();
    if( overlay.isValid() )
    {
      overlay->deleteLater();
      size = overlay->getSize();
      rPos = overlay->getTile().getIJ();
    }

    //bool deleteRoad = false;

    TilemapArea clearedTiles = tmap.getFilledRectangle( rPos, size );
    foreach( Tile* tile, clearedTiles )
    {
      BuildingType dstr2constr[] = { B_BURNING_RUINS, B_COLLAPSED_RUINS, B_PLAGUE_RUINS };
      bool canCreate = ConstructionManager::getInstance().canCreate( dstr2constr[_type] );
      if( canCreate )
      {
        GameEventMgr::append( BuildEvent::create( tile->getIJ(), dstr2constr[_type] ) );
      }
    }

    std::string dstr2string[] = { _("##alarm_fire_in_city##"), _("##alarm_building_collapsed##"),
                                  _("##alarm_plague_in_city##") };
    game.getCity()->onDisasterEvent().emit( _pos, dstr2string[_type] );
  }
}

GameEventPtr BuildEvent::create( const TilePos& pos, const BuildingType type )
{
  return create( pos, ConstructionManager::getInstance().create( type ) );
}

GameEventPtr BuildEvent::create(const TilePos& pos, ConstructionPtr building)
{
  BuildEvent* ev = new BuildEvent();
  ev->_pos = pos;
  ev->_building = building;

  GameEventPtr ret( ev );
  ret->drop();

  return ret;
}

void BuildEvent::exec( Game& game )
{
  const BuildingData& buildingData = BuildingDataHolder::instance().getData( _building->getType() );
  if( _building.isValid() )
  {
    _building->build( game.getCity(), _pos );

    game.getCity()->addOverlay( _building.as<LandOverlay>() );
    game.getCity()->getFunds().resolveIssue( FundIssue( CityFunds::buildConstruction, -buildingData.getCost() ) );

    if( _building->isNeedRoadAccess() && _building->getAccessRoads().empty() )
    {
      game.getCity()->onWarningMessage().emit( "##building_need_road_access##" );
    }
  }
}

GameEventPtr ClearLandEvent::create(const TilePos& pos)
{
  ClearLandEvent* ev = new ClearLandEvent();
  ev->_pos = pos;

  GameEventPtr ret( ev );
  ret->drop();

  return ret;
}

void ClearLandEvent::exec( Game& game )
{
  Tilemap& tmap = game.getCity()->getTilemap();

  Tile& cursorTile = tmap.at( _pos );

  if( cursorTile.getFlag( Tile::isDestructible ) )
  {
    Size size( 1 );
    TilePos rPos = _pos;

    LandOverlayPtr overlay = cursorTile.getOverlay();

    bool deleteRoad = false;

    if (cursorTile.getFlag( Tile::tlRoad )) deleteRoad = true;

    if ( overlay.isValid() )
    {
      size = overlay->getSize();
      rPos = overlay->getTile().getIJ();
      overlay->deleteLater();
    }

    TilemapArea clearedTiles = tmap.getFilledRectangle( rPos, size );
    foreach( Tile* tile, clearedTiles )
    {
      tile->setMasterTile( NULL );
      tile->setFlag( Tile::tlTree, false);
      tile->setFlag( Tile::tlBuilding, false);
      tile->setFlag( Tile::tlRoad, false);
      tile->setFlag( Tile::tlGarden, false);
      tile->setOverlay( NULL );

      deleteRoad |= tile->getFlag( Tile::tlRoad );

      if( tile->getFlag( Tile::tlMeadow ) )
      {
        tile->setPicture( TileHelper::convId2PicName( tile->getOriginalImgId() ) );
      }
      else
      {
        // choose a random landscape picture:
        // flat land1a 2-9;
        // wheat: land1a 18-29;
        // green_something: land1a 62-119;  => 58
        // green_flat: land1a 232-289; => 58

        // choose a random background image, green_something 62-119 or green_flat 232-240
         // 30% => choose green_sth 62-119
        // 70% => choose green_flat 232-289
        int startOffset  = ( (rand() % 10 > 6) ? 62 : 232 );
        int imgId = rand() % 58;

        tile->setPicture( ResourceGroup::land1a, startOffset + imgId );
      }
    }

    // recompute roads;
    // there is problem that we NEED to recompute all roads map for all buildings
    // because MaxDistance2Road can be any number
    //
    if( deleteRoad )
    {
      game.getCity()->updateRoads();
    }
  }
}

GameEventPtr ShowInfoboxEvent::create( const std::string& title, const std::string& text )
{
  ShowInfoboxEvent* ev = new ShowInfoboxEvent();
  ev->_title = title;
  ev->_text = text;

  GameEventPtr ret( ev );
  ret->drop();

  return ret;
}

void ShowInfoboxEvent::exec( Game& game )
{
  InfoBoxText* msgWnd = new InfoBoxText( game.getGui()->getRootWidget(), _title, _text );
  msgWnd->show();
}


GameEventPtr TogglePause::create()
{
  GameEventPtr ret( new TogglePause() );
  ret->drop();
  return ret;
}

void TogglePause::exec(Game& game)
{
  Widget* rootWidget = game.getGui()->getRootWidget();
  Label* wdg = safety_cast< Label* >( rootWidget->findChild( windowGamePausedId ) );
  game.setPaused( !game.isPaused() );

  if( game.isPaused()  )
  {
    if( !wdg )
    {
      Size scrSize = rootWidget->getSize();
      wdg = new Label( rootWidget, Rect( Point( (scrSize.getWidth() - 450)/2, 40 ), Size( 450, 50 ) ),
                       _("##game_is_paused##"), false, Label::bgWhiteFrame, windowGamePausedId );
      wdg->setTextAlignment( alignCenter, alignCenter );
    }
  }
  else
  {
    if( wdg )
    {
      wdg->deleteLater();
    }
  }
}


GameEventPtr ChangeSpeed::create(int value)
{
  ChangeSpeed* ev = new ChangeSpeed();
  ev->_value = value;
  GameEventPtr ret( ev );
  ret->drop();
  return ret;
}

void ChangeSpeed::exec(Game& game)
{
  game.changeTimeMultiplier( _value );
}


GameEventPtr FundIssueEvent::create(int type, int value)
{
  FundIssueEvent* ev = new FundIssueEvent();
  ev->_value = value;
  ev->_type = type;
  GameEventPtr ret( ev );
  ret->drop();
  return ret;
}

GameEventPtr FundIssueEvent::import(Good::Type good, int qty)
{
  FundIssueEvent* ev = new FundIssueEvent();
  ev->_gtype = good;
  ev->_qty = qty;
  ev->_type = CityFunds::importGoods;
  GameEventPtr ret( ev );
  ret->drop();
  return ret;
}

GameEventPtr FundIssueEvent::exportg(Good::Type good, int qty)
{
  FundIssueEvent* ev = new FundIssueEvent();
  ev->_gtype = good;
  ev->_qty = qty;
  ev->_type = CityFunds::exportGoods;
  GameEventPtr ret( ev );
  ret->drop();
  return ret;
}

void FundIssueEvent::exec(Game& game)
{
  if( _type == CityFunds::importGoods )
  {
    int price = game.getCity()->getTradeOptions().getSellPrice( _gtype );
    _value = -price * _qty / 100;
  }
  else if( _type == CityFunds::exportGoods )
  {
    int price = game.getCity()->getTradeOptions().getBuyPrice( _gtype );
    _value = price * _qty / 100;
  }

  game.getCity()->getFunds().resolveIssue( FundIssue( _type, _value ) );
}


GameEventPtr ShowEmpireMapWindow::create(bool show)
{
  ShowEmpireMapWindow* ev = new ShowEmpireMapWindow();
  ev->_show = show;
  GameEventPtr ret( ev );
  ret->drop();
  return ret;
}

void ShowEmpireMapWindow::exec(Game& game)
{
  List<EmpireMapWindow*> wndList = game.getGui()->getRootWidget()->findChildren<EmpireMapWindow*>();

  if( _show )
  {
    if( !wndList.empty() )
    {
      wndList.front()->bringToFront();
    }
    else
    {
      EmpireMapWindow::create( game.getEmpire(), game.getCity(), game.getGui()->getRootWidget(), -1 );
    }
  }
  else
  {
    if( !wndList.empty() )
    {
      wndList.front()->deleteLater();
    }
  }
}


GameEventPtr ShowAdvisorWindow::create(bool show, int advisor)
{
  ShowAdvisorWindow* ev = new ShowAdvisorWindow();
  ev->_show = show;
  ev->_advisor = advisor;
  GameEventPtr ret( ev );
  ret->drop();
  return ret;
}

void ShowAdvisorWindow::exec(Game& game)
{
  List<AdvisorsWindow*> wndList = game.getGui()->getRootWidget()->findChildren<AdvisorsWindow*>();

  if( _show )
  {
    if( !wndList.empty() )
    {
      wndList.front()->bringToFront();
      wndList.front()->showAdvisor( (AdvisorType)_advisor );
    }
    else
    {
      AdvisorsWindow::create( game.getGui()->getRootWidget(), -1, (AdvisorType)_advisor, game.getCity() );
    }
  }
  else
  {
    if( !wndList.empty() )
    {
      wndList.front()->deleteLater();
    }
  }
}
