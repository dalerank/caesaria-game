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
  TerrainTile& terrain = tmap.at( _pos ).getTerrain();
  TilePos rPos = _pos;

  if( terrain.isDestructible() )
  {
    Size size( 1 );

    LandOverlayPtr overlay = terrain.getOverlay();
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
        BuildEvent::create( tile->getIJ(), dstr2constr[_type] );
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
  TerrainTile& terrain = cursorTile.getTerrain();

  if( terrain.isDestructible() )
  {
    Size size( 1 );
    TilePos rPos = _pos;

    LandOverlayPtr overlay = terrain.getOverlay();

    bool deleteRoad = false;

    if (terrain.isRoad()) deleteRoad = true;

    if ( overlay.isValid() )
    {
      size = overlay->getSize();
      rPos = overlay->getTile().getIJ();
      overlay->deleteLater();
    }

    TilemapArea clearedTiles = tmap.getFilledRectangle( rPos, size );
    foreach( Tile* tile, clearedTiles )
    {
      tile->setMasterTile(NULL);
      TerrainTile &terrain = tile->getTerrain();
      terrain.setTree(false);
      terrain.setBuilding(false);
      terrain.setRoad(false);
      terrain.setGarden(false);
      terrain.setOverlay(NULL);

      // choose a random landscape picture:
      // flat land1a 2-9;
      // wheat: land1a 18-29;
      // green_something: land1a 62-119;  => 58
      // green_flat: land1a 232-289; => 58

      // FIX: when delete building on meadow, meadow is replaced by common land tile
      if( terrain.isMeadow() )
      {
        unsigned int originId = terrain.getOriginalImgId();
        tile->setPicture( &Picture::load( TerrainTileHelper::convId2PicName( originId ) ) );
      }
      else
      {
        // choose a random background image, green_something 62-119 or green_flat 232-240
         // 30% => choose green_sth 62-119
        // 70% => choose green_flat 232-289
        int startOffset  = ( (rand() % 10 > 6) ? 62 : 232 );
        int imgId = rand() % 58;

        tile->setPicture( &Picture::load( "land1a", startOffset + imgId));
      }
    }

    // recompute roads;
    // there is problem that we NEED to recompute all roads map for all buildings
    // because MaxDistance2Road can be any number
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

void FundIssueEvent::exec(Game& game)
{
  game.getCity()->getFunds().resolveIssue( FundIssue( _type, _value ) );
}
