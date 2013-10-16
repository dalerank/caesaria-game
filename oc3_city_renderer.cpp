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

#include "oc3_city_renderer.hpp"

#include <list>
#include <vector>

#include "oc3_tile.hpp"
#include "oc3_gfx_engine.hpp"
#include "oc3_resourcegroup.hpp"
#include "oc3_positioni.hpp"
#include "oc3_pictureconverter.hpp"
#include "oc3_event.hpp"
#include "oc3_roadbuild_helper.hpp"
#include "oc3_tilemapchangecommand.hpp"
#include "oc3_tilemap.hpp"
#include "oc3_stringhelper.hpp"
#include "oc3_building_house.hpp"
#include "oc3_house_level.hpp"
#include "oc3_building_watersupply.hpp"
#include "oc3_foreach.hpp"
#include "oc3_game_event_mgr.hpp"
#include "oc3_font.hpp"
#include "oc3_gfx_sdl_engine.hpp"
#include "oc3_gettext.hpp"

namespace WalkersVisibility
{
  static const WalkerType all[] = { WT_ALL, WT_NONE };
  static const WalkerType nobody[] = { WT_NONE };
  static const WalkerType engineer[] = { WT_ENGINEER, WT_NONE };
  static const WalkerType prefect[] = { WT_PREFECT, WT_NONE };
  static const WalkerType prestige[] = { WT_ALL, WT_NONE };
  static const WalkerType food[] = { WT_CART_PUSHER, WT_MARKETLADY, WT_MARKETLADY_HELPER, WT_NONE };
  static const WalkerType religion[] = { WT_ALL, WT_NONE };
  static const WalkerType allEntertainment[] = { WT_ACTOR, WT_GLADIATOR, WT_TAMER, WT_CHARIOT, WT_NONE };
  static const WalkerType actorsOnly[] = { WT_ACTOR, WT_NONE };
  static const WalkerType gladiatorsOnly[] = { WT_GLADIATOR, WT_NONE };
  static const WalkerType tamersOnly[] = { WT_TAMER, WT_NONE };
  static const WalkerType chariotsOnly[] = { WT_CHARIOT, WT_NONE };
  static const WalkerType doctorsOnly[] = { WT_DOCTOR, WT_NONE };
  static const WalkerType hospitalOnly[] = { WT_SURGEON, WT_NONE };
  static const WalkerType barberOnly[] = { WT_BARBER, WT_NONE };
  static const WalkerType bathsOnly[] = { WT_BATHLADY, WT_NONE };
}

class CityRenderer::Impl
{
public: 
  Picture clearPic;
  TilemapTiles postTiles;  // these tiles have draw over "normal" tilemap tiles!
  Point lastCursorPos;
  Point startCursorPos;
  bool  lmbPressed;
  // current map offset, for private use
  Point mapOffset;
  CityPtr city;     // city to display
  Tilemap* tilemap;
  GfxEngine* engine;
  TilemapCamera camera;  // visible map area
  std::set<int> overlayRendeFlags;
  int scrollSpeed;

  TilePos lastTilePos;
  TilemapChangeCommandPtr changeCommand;

  typedef Delegate1< Tile& > DrawTileSignature;
  DrawTileSignature drawTileFunction;

  Font debugFont;

  void getSelectedArea( TilePos& outStartPos, TilePos& outStopPos );
  // returns the tile at the cursor position.

  void buildAll();
  void clearAll();

  void drawTileBase( Tile& tile );
  void drawTileWater( Tile& tile );
  void drawTileDesirability( Tile& tile );
  void drawTileFire( Tile& tile );
  void drawTileEntertainment( Tile& tile );
  void drawTileDamage( Tile& tile );
  void drawTileHealth( Tile& tile );
  void drawTileReligion( Tile& tile );
  void drawTileInSelArea( Tile& tile, Tile* master );
  void drawTileFood( Tile& tile );
  void drawAnimations( LandOverlayPtr overlay, const Point& screenPos );
  void drawColumn( const Point& pos, const int startPicId, const int percent );

  void drawTilemapWithRemoveTools();
  void simpleDrawTilemap();

  void drawTile( Tile& tile );
  void drawTileEx( Tile& tile, const int depth );

  Tile* getTile( const Point& pos, bool overborder);

  WalkerList getVisibleWalkerList();
  void drawWalkersBetweenZ( WalkerList walkerList, int minZ, int maxZ );
  void drawBuildingAreaTiles( Tile& baseTile, LandOverlayPtr overlay, std::string resourceGroup, int tileId );

  template< class X, class Y >
  void setDrawFunction( Y* obj, void (X::*func)( Tile& ) )
  {
    drawTileFunction = makeDelegate( obj, func );
  }

  void resetWasDrawn( TilemapArea tiles )
  {
    foreach( Tile* tile, tiles )
      tile->resetWasDrawn();
  }

  void setVisibleWalkers(const WalkerType walkersTypes[])
  {
    visibleWalkers.clear();
    int i=0;
    while (walkersTypes[i] != WT_NONE)
    {
      visibleWalkers.push_back(walkersTypes[i++]);
    }
  }

  std::vector<WalkerType> visibleWalkers;

oc3_signals public:
  Signal1< const Tile& > onShowTileInfoSignal;
  Signal1< std::string > onWarningMessageSignal;
};

CityRenderer::CityRenderer() : _d( new Impl )
{
}

CityRenderer::~CityRenderer() {}

void CityRenderer::initialize( CityPtr city, GfxEngine* engine )
{
  _d->visibleWalkers.push_back(WT_ALL);
  _d->debugFont = Font::create( FONT_2_WHITE );
  _d->scrollSpeed = 4;
  _d->city = 0;
  _d->city = city;
  _d->tilemap = &city->getTilemap();
  _d->camera.init( *_d->tilemap );
  _d->engine = engine;
  _d->clearPic = Picture::load( "oc3_land", 2 );
  _d->setDrawFunction( _d.data(), &Impl::drawTileBase );
}

void CityRenderer::Impl::drawTileEx( Tile& tile, const int depth )
{
  if( tile.isFlat() )
  {
    return;  // tile has already been drawn!
  }

  Tile* master = tile.getMasterTile();

  if( 0 == master )    // single-tile
  {
    drawTile( tile );
    return;
  }

  // multi-tile: draw the master tile.
  // and it is time to draw the master tile
  if( master->getIJ().getZ() == depth && !master->getFlag( Tile::wasDrawn ) )
  {
    drawTile( *master );
  }
}

void CityRenderer::Impl::drawTile( Tile& tile )
{
  drawTileFunction( tile );
}

void CityRenderer::Impl::drawAnimations( LandOverlayPtr overlay, const Point& screenPos )
{
  // building foregrounds and animations
  const PicturesArray& fgPictures = overlay->getForegroundPictures();
  for( PicturesArray::const_iterator it=fgPictures.begin(); it != fgPictures.end(); it++ )
  {
    // skip void picture
    if( it->isValid() )
    {
      engine->drawPicture( *it, screenPos);
    }
  }
}

void CityRenderer::Impl::drawTileDesirability( Tile& tile )
{
  Point screenPos = tile.getXY() + mapOffset;

  tile.setWasDrawn();

  if( tile.getOverlay().isNull() )
  {
    //draw background
    if( tile.getFlag( Tile::isConstructible ) && tile.getDesirability() != 0 )
    {
      int picOffset = tile.getDesirability() < 0
                          ? math::clamp( tile.getDesirability() / 25, -3, 0 )
                          : math::clamp( tile.getDesirability() / 15, 0, 6 );
      Picture& pic = Picture::load( ResourceGroup::land2a, 37 + picOffset );

      engine->drawPicture( pic, screenPos );
    }
    else
    {
      engine->drawPicture( tile.getPicture(), screenPos );
    }    
  }
  else
  {   
    LandOverlayPtr overlay = tile.getOverlay();
    switch( overlay->getType() )
    {
    //roads
    case B_ROAD:
    case B_PLAZA:
      engine->drawPicture( tile.getPicture(), screenPos );
      drawAnimations( overlay, screenPos );
    break;  

    //other buildings
    default:      
      {
        int picOffset = tile.getDesirability() < 0
                          ? math::clamp( tile.getDesirability() / 25, -3, 0 )
                          : math::clamp( tile.getDesirability() / 15, 0, 6 );
        Picture& pic = Picture::load( ResourceGroup::land2a, 37 + picOffset );
        TilemapTiles tiles4clear = tilemap->getArea( tile.getIJ(), overlay->getSize() );
        foreach( Tile* tile, tiles4clear )
        {
          engine->drawPicture( pic, tile->getXY() + mapOffset );
        }
      }
    break;
    } 
  }

  if( tile.getDesirability() != 0 )
  {
    GfxSdlEngine* painter = static_cast< GfxSdlEngine* >( engine );
    debugFont.draw( painter->getScreen(), StringHelper::format( 0xff, "%d", tile.getDesirability() ), screenPos + Point( 20, -15 ), false );
  }
}

void CityRenderer::Impl::drawTileFire( Tile& tile )
{
  Point screenPos = tile.getXY() + mapOffset;

  tile.setWasDrawn();

  bool needDrawAnimations = false;
  if( tile.getOverlay().isNull() )
  {
    //draw background
    engine->drawPicture( tile.getPicture(), screenPos );
  }
  else
  {   
    LandOverlayPtr overlay = tile.getOverlay();
    int fireLevel = 0;
    switch( overlay->getType() )
    {
    //fire buildings and roads
    case B_ROAD:
    case B_PLAZA:
    case B_BURNING_RUINS:
    case B_BURNED_RUINS:
    case B_COLLAPSED_RUINS:
    case B_PREFECTURE:
      engine->drawPicture( tile.getPicture(), screenPos );
      needDrawAnimations = true;
    break;  

      //houses
    case B_HOUSE:
      {
        HousePtr house = overlay.as< House >();
        fireLevel = (int)house->getFireLevel();
        needDrawAnimations = (house->getLevelSpec().getHouseLevel() == 1) && (house->getHabitants().size() ==0);

        drawBuildingAreaTiles( overlay->getTile(), overlay, ResourceGroup::foodOverlay, OverlayPic::inHouseBase  );
      }
    break;

      //other buildings
    default:
      {
        BuildingPtr building = overlay.as< Building >();
        if( building != 0 )
        {
          fireLevel = (int)building->getFireLevel();
        }

        drawBuildingAreaTiles( overlay->getTile(), overlay, ResourceGroup::foodOverlay, OverlayPic::base  );
      }
    break;
    }  

    if( needDrawAnimations )
    {
      drawAnimations( overlay, screenPos );
    }
    else if( fireLevel >= 0)
    {
      drawColumn( screenPos, 18, fireLevel );
    }
  }
}

void CityRenderer::Impl::drawTileDamage( Tile& tile )
{
  Point screenPos = tile.getXY() + mapOffset;

  tile.setWasDrawn();

  bool needDrawAnimations = false;
  if( tile.getOverlay().isNull() )
  {
    //draw background
    engine->drawPicture( tile.getPicture(), screenPos );
  }
  else
  {   
    LandOverlayPtr overlay = tile.getOverlay();
    int damageLevel = 0;
    switch( overlay->getType() )
    {
      //fire buildings and roads
    case B_ROAD:
    case B_PLAZA:
    case B_COLLAPSED_RUINS:
    case B_ENGINEER_POST:
      needDrawAnimations = true;
      engine->drawPicture( tile.getPicture(), screenPos );
      break;  

      //houses
    case B_HOUSE:
      {
        HousePtr house = overlay.as< House >();
        damageLevel = (int)house->getDamageLevel();
        needDrawAnimations = (house->getLevelSpec().getHouseLevel() == 1) && (house->getHabitants().size() == 0);
        drawBuildingAreaTiles( overlay->getTile(), overlay, ResourceGroup::foodOverlay, OverlayPic::inHouseBase );
      }
      break;

      //other buildings
    default:
      {
        BuildingPtr building = overlay.as< Building >();
        if( building.isValid() )
        {
          damageLevel = (int)building->getDamageLevel();
        }

        drawBuildingAreaTiles( overlay->getTile(), overlay, ResourceGroup::foodOverlay, OverlayPic::base );
      }
      break;
    }      

    if( needDrawAnimations )
    {
      drawAnimations( overlay, screenPos );
    }
    else if( damageLevel >= 0 )
    {
      drawColumn( screenPos, 15, damageLevel );
    }
  }
}

void CityRenderer::Impl::drawTileEntertainment( Tile& tile )
{
  Point screenPos = tile.getXY() + mapOffset;

  tile.setWasDrawn();

  bool needDrawAnimations = false;
  if( tile.getOverlay().isNull() )
  {
    //draw background
    engine->drawPicture( tile.getPicture(), screenPos );
  }
  else
  {
    LandOverlayPtr overlay = tile.getOverlay();

    int entertainmentLevel = -1;
    switch( overlay->getType() )
    {
      //fire buildings and roads
    case B_ROAD:
    case B_PLAZA:
      needDrawAnimations = true;
      engine->drawPicture( tile.getPicture(), screenPos );
    break;

    case B_THEATER:
    case buildingAmphitheater:
    case B_COLLOSSEUM:
    case B_HIPPODROME:
    case B_LION_HOUSE:
    case B_ACTOR_COLONY:
    case B_GLADIATOR_SCHOOL:
      needDrawAnimations = overlayRendeFlags.count( overlay->getType() );
      if( needDrawAnimations )
      {
        engine->drawPicture( tile.getPicture(), screenPos );
      }
      else
      {
        drawBuildingAreaTiles( overlay->getTile(), overlay, ResourceGroup::foodOverlay, OverlayPic::base );
      }
    break;

      //houses
    case B_HOUSE:
      {
        HousePtr house = overlay.as< House >();
        if( overlayRendeFlags.count( B_MAX ) ) { entertainmentLevel = house->getLevelSpec().computeEntertainmentLevel( house ); }
        else if( overlayRendeFlags.count( B_THEATER ) ) { entertainmentLevel = house->getServiceValue( Service::theater ); }
        else if( overlayRendeFlags.count( buildingAmphitheater ) ) { entertainmentLevel = house->getServiceValue( Service::amphitheater ); }
        else if( overlayRendeFlags.count( B_COLLOSSEUM ) ) { entertainmentLevel = house->getServiceValue( Service::colloseum ); }
        else if( overlayRendeFlags.count( B_HIPPODROME ) ) { entertainmentLevel = house->getServiceValue( Service::hippodrome ); }

        needDrawAnimations = (house->getLevelSpec().getHouseLevel() == 1) && (house->getHabitants().size() == 0);
        drawBuildingAreaTiles( overlay->getTile(), overlay, ResourceGroup::foodOverlay, OverlayPic::inHouseBase );
      }
    break;

      //other buildings
    default:
      {
        drawBuildingAreaTiles( overlay->getTile(), overlay, ResourceGroup::foodOverlay, OverlayPic::base );
      }
    break;
    }

    if( needDrawAnimations )
    {
      drawAnimations( overlay, screenPos );
    }
    else if( entertainmentLevel > 0 )
    {
      drawColumn( screenPos, 9, entertainmentLevel );
    }
  }
}

void CityRenderer::Impl::drawTileHealth( Tile& tile )
{
  Point screenPos = tile.getXY() + mapOffset;

  tile.setWasDrawn();

  bool needDrawAnimations = false;
  if( tile.getOverlay().isNull() )
  {
    //draw background
    engine->drawPicture( tile.getPicture(), screenPos );
  }
  else
  {
    LandOverlayPtr overlay = tile.getOverlay();

    int healthLevel = -1;
    switch( overlay->getType() )
    {
      //fire buildings and roads
    case B_ROAD:
    case B_PLAZA:
      needDrawAnimations = true;
      engine->drawPicture( tile.getPicture(), screenPos );
    break;

    case B_DOCTOR:
    case B_HOSPITAL:
    case B_BARBER:
    case B_BATHS:
      needDrawAnimations = overlayRendeFlags.count( overlay->getType() );
      if( needDrawAnimations )
      {
        engine->drawPicture( tile.getPicture(), screenPos );
      }
      else
      {
        drawBuildingAreaTiles( overlay->getTile(), overlay, ResourceGroup::foodOverlay, OverlayPic::base );
      }
    break;

      //houses
    case B_HOUSE:
      {
        HousePtr house = overlay.as< House >();

        if( overlayRendeFlags.count( B_DOCTOR ) ) { healthLevel = house->getHealthLevel(); }
        else if( overlayRendeFlags.count( B_HOSPITAL ) ) { healthLevel = house->getServiceValue( Service::hospital ); }
        else if( overlayRendeFlags.count( B_BARBER ) ) { healthLevel = house->getServiceValue( Service::barber ); }
        else if( overlayRendeFlags.count( B_BATHS ) ) { healthLevel = house->getServiceValue( Service::baths ); }

        needDrawAnimations = (house->getLevelSpec().getHouseLevel() == 1) && (house->getHabitants().size() == 0);

        drawBuildingAreaTiles( overlay->getTile(), overlay, ResourceGroup::foodOverlay, OverlayPic::inHouseBase );
      }
    break;

      //other buildings
    default:
      {
        drawBuildingAreaTiles( overlay->getTile(), overlay, ResourceGroup::foodOverlay, OverlayPic::base );
      }
    break;
    }

    if( needDrawAnimations )
    {
      drawAnimations( overlay, screenPos );
    }
    else if( healthLevel > 0 )
    {
      drawColumn( screenPos, 9, healthLevel );
    }
  }
}


void CityRenderer::Impl::drawTileReligion( Tile& tile )
{
  Point screenPos = tile.getXY() + mapOffset;

  tile.setWasDrawn();

  bool needDrawAnimations = false;
  if( tile.getOverlay().isNull() )
  {
    //draw background
    engine->drawPicture( tile.getPicture(), screenPos );
  }
  else
  {   
    LandOverlayPtr overlay = tile.getOverlay();

    int religionLevel = -1;
    switch( overlay->getType() )
    {
      //fire buildings and roads
    case B_ROAD:
    case B_PLAZA:
    case B_TEMPLE_CERES: case B_TEMPLE_MARS: case B_TEMPLE_MERCURE: case B_TEMPLE_NEPTUNE: case B_TEMPLE_VENUS:
    case B_TEMPLE_ORACLE:
    case B_BIG_TEMPLE_CERES: case B_BIG_TEMPLE_MARS: case B_BIG_TEMPLE_MERCURE: case B_BIG_TEMPLE_NEPTUNE: case B_BIG_TEMPLE_VENUS:
      needDrawAnimations = true;
      engine->drawPicture( tile.getPicture(), screenPos );
    break;  

      //houses
    case B_HOUSE:
      {
        HousePtr house = overlay.as< House >();
        religionLevel = house->getServiceValue(Service::religionMercury);
        religionLevel += house->getServiceValue(Service::religionVenus);
        religionLevel += house->getServiceValue(Service::religionMars);
        religionLevel += house->getServiceValue(Service::religionNeptune);
        religionLevel += house->getServiceValue(Service::religionCeres);
        religionLevel = math::clamp( religionLevel / (house->getLevelSpec().getMinReligionLevel()+1), 0, 100 );
        needDrawAnimations = (house->getLevelSpec().getHouseLevel() == 1) && (house->getHabitants().size() ==0);

        drawBuildingAreaTiles( overlay->getTile(), overlay, ResourceGroup::foodOverlay, OverlayPic::inHouseBase );
      }
    break;

      //other buildings
    default:
      {
        drawBuildingAreaTiles( overlay->getTile(), overlay, ResourceGroup::foodOverlay, OverlayPic::base );
      }
    break;
    }  

    if( needDrawAnimations )
    {
      drawAnimations( overlay, screenPos );
    }
    else if( religionLevel > 0 )
    {
      drawColumn( screenPos, 9, religionLevel );
    }
  }
}

void CityRenderer::Impl::drawTileFood( Tile& tile )
{
  Point screenPos = tile.getXY() + mapOffset;

  tile.setWasDrawn();

  bool needDrawAnimations = false;
  if( tile.getOverlay().isNull() )
  {
    //draw background
    engine->drawPicture( tile.getPicture(), screenPos );
  }
  else
  {   
    LandOverlayPtr overlay = tile.getOverlay();
    Picture pic;
    int foodLevel = -1;
    switch( overlay->getType() )
    {
      //fire buildings and roads
    case B_ROAD:
    case B_PLAZA:
    case B_MARKET:
    case B_GRANARY:
      pic = tile.getPicture();
      needDrawAnimations = true;
    break;  

      //houses
    case B_HOUSE:
      {
        drawBuildingAreaTiles(tile, overlay, ResourceGroup::foodOverlay, OverlayPic::inHouseBase );
        HousePtr house = overlay.as< House >();
        foodLevel = house->getFoodLevel();
        needDrawAnimations = (house->getLevelSpec().getHouseLevel() == 1) && (house->getHabitants().size() == 0);
      }
      break;

      //other buildings
    default:
      {
        drawBuildingAreaTiles(tile, overlay, ResourceGroup::foodOverlay, OverlayPic::base);
      }
      break;
    }  

    if ( pic.isValid())
    {
      engine->drawPicture( pic, screenPos );
    }

    if( needDrawAnimations )
    {
      drawAnimations( overlay, screenPos );
    }
    else if( foodLevel >= 0 )
    {
      drawColumn( screenPos, 18, math::clamp( 100 - foodLevel, 0, 100 ) );
    }
  }
}

void CityRenderer::Impl::drawTileWater( Tile& tile )
{
  Point screenPos = tile.getXY() + mapOffset;

  tile.setWasDrawn();

  bool needDrawAnimations = false;
  Size areaSize(1);

  if( tile.getOverlay().isNull() )
  {
    //draw background
    engine->drawPicture( tile.getPicture(), screenPos );
  }
  else
  {
    LandOverlayPtr overlay = tile.getOverlay();
    Picture pic;
    switch( overlay->getType() )
    {
      //water buildings
    case B_ROAD:
    case B_PLAZA:
    case B_RESERVOIR:
    case B_FOUNTAIN:
    case B_WELL:
    case B_AQUEDUCT:
      pic = tile.getPicture();
      needDrawAnimations = true;
      areaSize = overlay->getSize();
    break;

    default:
    {
      int tileNumber = 0;
      bool haveWater = tile.getWaterService( WTR_FONTAIN ) > 0 || tile.getWaterService( WTR_WELL ) > 0;
      if ( overlay->getType() == B_HOUSE )
      {
        HousePtr h = overlay.as<House>();
        tileNumber = OverlayPic::inHouse;
        haveWater = haveWater || h->hasServiceAccess(Service::fontain) || h->hasServiceAccess(Service::well);
      }
      tileNumber += (haveWater ? OverlayPic::haveWater : 0);
      tileNumber += tile.getWaterService( WTR_RESERVOIR ) > 0 ? OverlayPic::reservoirRange : 0;

      drawBuildingAreaTiles( tile, overlay, ResourceGroup::waterOverlay, OverlayPic::base + tileNumber );

      pic = Picture::getInvalid();
      areaSize = 0;
      needDrawAnimations = false;
    }
    break;
    }

    if ( pic.isValid() )
    {
      engine->drawPicture( pic, screenPos );
    }

    if( needDrawAnimations )
    {
      drawAnimations( overlay, screenPos );
    }
  }

  if( !needDrawAnimations && ( tile.isWalkable(true) || tile.getFlag( Tile::tlBuilding ) ) )
  {
    TilemapArea area = tilemap->getArea( tile.getIJ(), areaSize );

    foreach( Tile* rtile, area )
    {
      int reservoirWater = rtile->getWaterService( WTR_RESERVOIR );
      int fontainWater = rtile->getWaterService( WTR_FONTAIN );

      if( (reservoirWater + fontainWater > 0) && ! rtile->getFlag( Tile::tlWater ) && rtile->getOverlay().isNull() )
      {
        int picIndex = reservoirWater ? OverlayPic::reservoirRange : 0;
        picIndex |= fontainWater > 0 ? OverlayPic::haveWater : 0;
        picIndex |= OverlayPic::skipLeftBorder | OverlayPic::skipRightBorder;
        engine->drawPicture( Picture::load( ResourceGroup::waterOverlay, picIndex + OverlayPic::base ), rtile->getXY() + mapOffset );
      }
    }
  }
}

void CityRenderer::Impl::drawTileBase( Tile& tile )
{
  Point screenPos = tile.getXY() + mapOffset;

  LandOverlayPtr overlay = tile.getOverlay();

  if( overlay.isValid())
  {
    if (overlay.is<Aqueduct>() && postTiles.size() > 0)
    {
      // check, do we have any aqueducts there... there can be empty items
      bool isAqueducts = false;
      foreach( Tile* tile, postTiles )
      {
        if( tile->getOverlay().is<Aqueduct>() )
        {
          isAqueducts = true;
          break;
        }
      }

      if( isAqueducts )
      {
        tile.setWasDrawn();
        Picture& pic = overlay.as<Aqueduct>()->computePicture( city, &postTiles, tile.getIJ());
        engine->drawPicture( pic, screenPos );
      }
    }
  }

  if( !tile.getFlag( Tile::wasDrawn ) )
  {
    tile.setWasDrawn();
    engine->drawPicture( tile.getPicture(), screenPos );

    if( tile.getAnimation().isValid() )
    {
      engine->drawPicture( tile.getAnimation().getCurrentPicture(), screenPos );
    }
  }

  if( overlay.isNull() )
  {
    return;
  }

  drawAnimations( overlay, screenPos );
}

void CityRenderer::Impl::drawTileInSelArea( Tile& tile, Tile* master )
{
  if( master==NULL )
  {
    // single-tile
    drawTileFunction( tile );
    engine->drawPicture( clearPic, tile.getXY() + mapOffset );
  }
  else
  {
    engine->setTileDrawMask( 0x00ff0000, 0, 0, 0xff000000 );

    // multi-tile: draw the master tile.
    if( !master->getFlag( Tile::wasDrawn ) )
      drawTileFunction( *master );

    engine->resetTileDrawMask();
  }
}

void CityRenderer::Impl::drawTilemapWithRemoveTools()
{
  // center the map on the screen
  mapOffset = Point( engine->getScreenWidth() / 2 - 30 * (camera.getCenterX() + 1) + 1,
                     engine->getScreenHeight() / 2 + 15 * (camera.getCenterZ()-tilemap->getSize() + 1) - 30 );

  int lastZ = -1000;  // dummy value

  TilemapArea visibleTiles = camera.getTiles();
  resetWasDrawn( visibleTiles );

  TilePos startPos, stopPos;
  getSelectedArea( startPos, stopPos );
 
  std::set<int> hashDestroyArea;
  TilemapArea destroyArea = tilemap->getArea( startPos, stopPos );
  
  //create list of destroy tiles add full area building if some of it tile constain in destroy area
  foreach( Tile* tile, destroyArea)
  {
    hashDestroyArea.insert( tile->getJ() * 1000 + tile->getI() );

    LandOverlayPtr overlay = tile->getOverlay();
    if( overlay.isValid() )
    {
      TilemapArea overlayArea = tilemap->getArea( overlay->getTilePos(), overlay->getSize() );
      foreach( Tile* ovelayTile, overlayArea )
      {
        hashDestroyArea.insert( ovelayTile->getJ() * 1000 + ovelayTile->getI() );
      }
    }
  }
  //Rect destroyArea = Rect( startPos.getI(), startPos.getJ(), stopPos.getI(), stopPos.getJ() );

  // FIRST PART: draw all flat land (walkable/boatable)
  foreach( Tile* tile, visibleTiles )
  {
    Tile* master = tile->getMasterTile();

    if( !tile->isFlat() )
      continue;

    int tilePosHash = tile->getJ() * 1000 + tile->getI();
    if( hashDestroyArea.find( tilePosHash ) != hashDestroyArea.end() )
    {
      drawTileInSelArea( *tile, master );
    }
    else
    {
      if( master==NULL )
      {
        // single-tile
        drawTile( *tile );
      }
      else if( !master->getFlag( Tile::wasDrawn ) )
      {
        // multi-tile: draw the master tile.
        drawTile( *master );
      }    
    }
  }  

  // SECOND PART: draw all sprites, impassable land and buildings
  WalkerList walkerList = getVisibleWalkerList();
  foreach( Tile* tile, visibleTiles )
  {
    int z = tile->getIJ().getZ();

    if (z != lastZ)
    {
      // TODO: pre-sort all animations
      lastZ = z;
      this->drawWalkersBetweenZ(walkerList, z, z+1);
    }   

    int tilePosHash = tile->getJ() * 1000 + tile->getI();
    if( hashDestroyArea.find( tilePosHash ) != hashDestroyArea.end() )
    {
      engine->setTileDrawMask( 0x00ff0000, 0, 0, 0xff000000 );      
    }

    drawTileEx( *tile, z );
    engine->resetTileDrawMask();
  }
}

void CityRenderer::Impl::simpleDrawTilemap()
{
  // center the map on the screen
  mapOffset = Point( engine->getScreenWidth() / 2 - 30 * (camera.getCenterX() + 1) + 1,
                     engine->getScreenHeight() / 2 + 15 * (camera.getCenterZ() - tilemap->getSize() + 1) - 30 );

  int lastZ = -1000;  // dummy value

 TilemapArea visibleTiles = camera.getTiles();

  foreach( Tile* tile, visibleTiles )
  {
    tile->resetWasDrawn();
  }

  // FIRST PART: draw all flat land (walkable/boatable)
  foreach( Tile* tile, visibleTiles )
  {
    Tile* master = tile->getMasterTile();

    if( !tile->isFlat() )
      continue;

    if( master==NULL )
    {
      // single-tile
      drawTile( *tile );
    }
    else
    {
      // multi-tile: draw the master tile.
      if( !master->getFlag( Tile::wasDrawn ) )
        drawTile( *master );
    }    
  }  

  // SECOND PART: draw all sprites, impassable land and buildings
  WalkerList walkerList = getVisibleWalkerList();

  foreach( Tile* tile, visibleTiles )
  {
    int z = tile->getIJ().getZ();

    if (z != lastZ)
    {
      // TODO: pre-sort all animations
      lastZ = z;
      drawWalkersBetweenZ( walkerList, z, z+1);
    }   

    drawTileEx( *tile, z );
  }
}

void CityRenderer::draw()
{
  //First part: drawing city
  if( _d->changeCommand.isValid() && _d->changeCommand.is<TilemapRemoveCommand>() )
  {
    _d->drawTilemapWithRemoveTools();
  }
  else
  {
    _d->simpleDrawTilemap();
  }

  //Second part: drawing build tools
  if( _d->changeCommand.isValid() && _d->changeCommand.is<TilemapBuildCommand>() )
  {
    foreach( Tile* postTile, _d->postTiles )
    {
      postTile->resetWasDrawn();

      ConstructionPtr ptr_construction = postTile->getOverlay().as<Construction>();
      _d->engine->resetTileDrawMask();

      if (ptr_construction != NULL)
      {
        if (ptr_construction->canBuild( _d->city, postTile->getIJ()))
        {
          _d->engine->setTileDrawMask( 0x00000000, 0x0000ff00, 0, 0xff000000 );

          // aqueducts must be shown in correct form
          AqueductPtr aqueduct = ptr_construction.as<Aqueduct>();
          if (aqueduct != NULL)
            aqueduct->setPicture(aqueduct->computePicture( _d->city, &_d->postTiles, postTile->getIJ()));
        }
      }

      _d->drawTileEx( *postTile, postTile->getIJ().getZ() );
    }

    _d->engine->resetTileDrawMask();
  }
}

Tile* CityRenderer::getTile( const Point& pos, bool overborder )
{
  return _d->getTile( pos, overborder );
}

Tile* CityRenderer::Impl::getTile( const Point& pos, bool overborder)
{
  Point mOffset = pos - mapOffset;  // x relative to the left most pixel of the tilemap
  int i = (mOffset.getX() + 2 * mOffset.getY()) / 60;
  int j = (mOffset.getX() - 2 * mOffset.getY()) / 60;
  
  if( overborder )
  {
      i = math::clamp( i, 0, tilemap->getSize() - 1 );
      j = math::clamp( j, 0, tilemap->getSize() - 1 );
  }
  // std::cout << "ij ("<<i<<","<<j<<")"<<std::endl;

  if (i>=0 && j>=0 && i < tilemap->getSize() && j < tilemap->getSize())
  {
     // valid coordinate
     return &tilemap->at( TilePos( i, j ) );
  }
  else
  {
     // the pixel is outside the tilemap => no tile here
     return NULL;
  }
}

TilemapCamera& CityRenderer::getCamera()
{
  return _d->camera;
}

void CityRenderer::updatePreviewTiles( bool force )
{
  TilemapBuildCommandPtr bldCommand = _d->changeCommand.as<TilemapBuildCommand>();

  if( bldCommand.isNull() )
    return;

  if( !bldCommand->isMultiBuilding() )
    _d->startCursorPos = _d->lastCursorPos;

  Tile* curTile = getTile( _d->lastCursorPos, true );

  if( !curTile )
    return;

  if( curTile && !force && _d->lastTilePos == curTile->getIJ() )
    return;

  _d->lastTilePos = curTile->getIJ();

  discardPreview();

  if( bldCommand->isBorderBuilding() )
  {
    Tile* startTile = getTile( _d->startCursorPos, true );  // tile under the cursor (or NULL)
    Tile* stopTile  = getTile( _d->lastCursorPos,  true );

    ConstTilemapWay pathWay = RoadPropagator::createPath( *_d->tilemap, *startTile, *stopTile );
    for( ConstTilemapWay::iterator it=pathWay.begin(); it != pathWay.end(); it++ )
    {
      checkPreviewBuild( (*it)->getIJ() );
    }
  }
  else
  {
    TilePos startPos, stopPos;
    _d->getSelectedArea( startPos, stopPos );

    std::cout << "start is" << startPos << " and stop is " << stopPos << std::endl;

    for( int i = startPos.getI(); i <= stopPos.getI(); i++ )
    {
      for( int j = startPos.getJ(); j <=stopPos.getJ(); j++ )
      {
        checkPreviewBuild( TilePos( i, j ) );
      }
    }
  }
}

void CityRenderer::Impl::getSelectedArea( TilePos& outStartPos, TilePos& outStopPos )
{
  Tile* startTile = getTile( startCursorPos, true );  // tile under the cursor (or NULL)
  Tile* stopTile  = getTile( lastCursorPos, true );

  TilePos startPosTmp = startTile->getIJ();
  TilePos stopPosTmp  = stopTile->getIJ();

//  std::cout << "TilemapRenderer::_getSelectedArea" << " ";
//  std::cout << "(" << startPosTmp.getI() << " " << startPosTmp.getJ() << ") (" << stopPosTmp.getI() << " " << stopPosTmp.getJ() << ")" << std::endl;
    
  outStartPos = TilePos( std::min<int>( startPosTmp.getI(), stopPosTmp.getI() ), std::min<int>( startPosTmp.getJ(), stopPosTmp.getJ() ) );
  outStopPos  = TilePos( std::max<int>( startPosTmp.getI(), stopPosTmp.getI() ), std::max<int>( startPosTmp.getJ(), stopPosTmp.getJ() ) );
}

void CityRenderer::Impl::clearAll()
{
  TilePos startPos, stopPos;
  getSelectedArea( startPos, stopPos );

  TilemapTiles tiles4clear = tilemap->getArea( startPos, stopPos );
  foreach( Tile* tile, tiles4clear )
  {
    GameEventMgr::append( ClearLandEvent::create( tile->getIJ() ) );
  }
}

void CityRenderer::Impl::buildAll()
{
  TilemapBuildCommandPtr bldCommand = changeCommand.as<TilemapBuildCommand>();
  if( bldCommand.isNull() )
    return;

  ConstructionPtr cnstr = bldCommand->getContruction();

  if( !cnstr.isValid() )
  {
    StringHelper::debug( 0xff, "No construction for build" );
    return;
  }

  bool buildOk = false;
  foreach( Tile* tile, postTiles )
  {   
    if( cnstr->canBuild( city, tile->getIJ() ) && tile->isMasterTile())
    {
      GameEventMgr::append( BuildEvent::create( tile->getIJ(), cnstr->getType() ) );
      buildOk = true;
    }   
  }

  if( !buildOk )
  {
    std::string errorStr = cnstr->getError();

    onWarningMessageSignal.emit( errorStr.empty()
                                 ? _("##need_build_on_cleared_area##")
                                 : errorStr );
  }
}

void CityRenderer::Impl::drawColumn( const Point& pos, const int startPicId, const int percent )
{
  engine->drawPicture( Picture::load( ResourceGroup::sprites, startPicId + 2 ), pos + Point( 5, 15 ) );
  
  int roundPercent = ( percent / 10 ) * 10;
  Picture& pic = Picture::load( ResourceGroup::sprites, startPicId + 1 );
  for( int offsetY=10; offsetY < roundPercent; offsetY += 10 )
  {
    engine->drawPicture( pic, pos - Point( -13, -5 + offsetY ) );
  }

  if( percent >= 10 )
  {
    engine->drawPicture( Picture::load( ResourceGroup::sprites, startPicId ), pos - Point( -1, -6 + roundPercent ) );
  }
}

void CityRenderer::Impl::drawWalkersBetweenZ(WalkerList walkerList, int minZ, int maxZ)
{
  PicturesArray pictureList;

  foreach( WalkerPtr walker, walkerList )
  {
    // TODO: calculate once && sort
    int zAnim = walker->getIJ().getZ();// getJ() - walker.getI();
    if( zAnim > minZ && zAnim <= maxZ )
    {
      pictureList.clear();
      walker->getPictureList( pictureList );
      foreach( Picture& picRef, pictureList )
      {
        if( picRef.isValid() )
        {
          engine->drawPicture( picRef, walker->getPosition() + mapOffset );
        }
      }
    }
  }
}

void CityRenderer::Impl::drawBuildingAreaTiles(Tile& baseTile, LandOverlayPtr overlay, std::string resourceGroup, int tileId)
{
  TilemapArea area = tilemap->getArea( baseTile.getIJ(), overlay->getSize() );

  Picture *pic = NULL;
  int leftBorderAtI = baseTile.getI();
  int rightBorderAtJ = overlay->getSize().getHeight() - 1 + baseTile.getJ();
  foreach( Tile* tile, area )
  {
    int tileBorders = ( tile->getI() == leftBorderAtI ? 0 : OverlayPic::skipLeftBorder )
                        + ( tile->getJ() == rightBorderAtJ ? 0 : OverlayPic::skipRightBorder );
    pic = &Picture::load(resourceGroup, tileBorders + tileId);
    engine->drawPicture( *pic, tile->getXY() + mapOffset );
  }
}

WalkerList CityRenderer::Impl::getVisibleWalkerList()
{
  WalkerList walkerList;
  foreach( WalkerType wtAct, visibleWalkers )
  {
    WalkerList foundWalkers = city->getWalkerList( wtAct );
    walkerList.insert(walkerList.end(), foundWalkers.begin(), foundWalkers.end());
  }

  return walkerList;
}

void CityRenderer::handleEvent( NEvent& event )
{
    if( event.EventType == OC3_MOUSE_EVENT )
    {
        switch( event.MouseEvent.Event  )
        {
        case OC3_MOUSE_MOVED:
        {
            _d->lastCursorPos = event.MouseEvent.getPosition();  
            if( !_d->lmbPressed || _d->startCursorPos.getX() < 0 )
            {
                _d->startCursorPos = _d->lastCursorPos;
            }
           
            updatePreviewTiles();
        }
        break;        

        case OC3_LMOUSE_PRESSED_DOWN:
        {
            _d->startCursorPos = _d->lastCursorPos;
            _d->lmbPressed = true;
            updatePreviewTiles();
        }
        break;

        case OC3_LMOUSE_LEFT_UP:            // left button
        {
            Tile* tile = _d->getTile( event.MouseEvent.getPosition(), false );  // tile under the cursor (or NULL)
            if( tile == 0 )
            {
              _d->lmbPressed = false;
              break;
            }

            if( _d->changeCommand.isValid() )
            {                
                if( _d->changeCommand.is<TilemapRemoveCommand>() )
                {
                    _d->clearAll();                      
                }
                else if( _d->changeCommand.is<TilemapBuildCommand>() )
                {
                    _d->buildAll();               
                }
                _d->startCursorPos = _d->lastCursorPos;
                updatePreviewTiles( true );
            }
            else
            {
                getCamera().setCenter( tile->getIJ() );
                _d->city->setCameraPos( tile->getIJ() );
            }

            _d->lmbPressed = false;
            _d->startCursorPos = _d->lastCursorPos;
        }
        break;

        case OC3_RMOUSE_LEFT_UP:
        {
            Tile* tile = _d->getTile( event.MouseEvent.getPosition(), false );  // tile under the cursor (or NULL)
            if( _d->changeCommand.isValid() )
            { 
                _d->changeCommand = TilemapChangeCommandPtr();
                discardPreview();
            }
            else
            {
              if( tile )
              {
                _d->onShowTileInfoSignal.emit( *tile );
              }
            }         
        }
        break;

        default:
        break;
        }
    }  

    if( event.EventType == OC3_KEYBOARD_EVENT )
    {
      int moveValue = _d->scrollSpeed * ( event.KeyboardEvent.Shift ? 2 : 0 ) ;
      switch( event.KeyboardEvent.Key )
      {
      case KEY_UP: getCamera().moveUp( moveValue  ); break;
      case KEY_DOWN: getCamera().moveDown( moveValue ); break;
      case KEY_RIGHT: getCamera().moveRight( moveValue ); break;
      case KEY_LEFT: getCamera().moveLeft( moveValue ); break;
      default: break;
      }
    }
}

void CityRenderer::discardPreview()
{
  foreach( Tile* tile, _d->postTiles )
  {
    if( tile->getOverlay().isValid() )
    {
      tile->getOverlay()->deleteLater();
    }

    delete tile;
  }

  _d->postTiles.clear();
}



void CityRenderer::checkPreviewBuild(const TilePos & pos)
{
  TilemapBuildCommandPtr bldCommand = _d->changeCommand.as<TilemapBuildCommand>();

  if (bldCommand.isNull())
    return;

  // TODO: do only when needed, when (i, j, _buildInstance) has changed
  ConstructionPtr overlay = bldCommand->getContruction();

  if (!overlay.isValid()) {
    return;
  }

  int size = overlay->getSize().getWidth();

  if( overlay->canBuild( _d->city, pos ) )
  {
    bldCommand->setCanBuild(true);
    Tile *masterTile=0;
    for (int dj = 0; dj < size; ++dj)
    {
      for (int di = 0; di < size; ++di)
      {
        Tile* tile = new Tile(_d->tilemap->at( pos + TilePos( di, dj ) ));  // make a copy of tile

        if (di==0 && dj==0)
        {
          // this is the masterTile
          masterTile = tile;
        }
        tile->setPicture( &overlay->getPicture() );
        tile->setMasterTile( masterTile );
        tile->setFlag( Tile::tlBuilding, true );
        tile->setOverlay( overlay.as<LandOverlay>() );
        _d->postTiles.push_back( tile );
        //_priorityTiles.push_back( tile );
      }
    }
  }
  else
  {
    bldCommand->setCanBuild(false);

    Picture& grnPicture = Picture::load("oc3_land", 1);
    Picture& redPicture = Picture::load("oc3_land", 2);

    for (int dj = 0; dj < size; ++dj)
    {
      for (int di = 0; di < size; ++di)
      {
        TilePos rPos = pos + TilePos( di, dj );
        if( !_d->tilemap->isInside( rPos ) )
          continue;

        Tile* tile = new Tile( _d->tilemap->at( rPos ) );  // make a copy of tile

        bool isConstructible = tile->getFlag( Tile::isConstructible );
        tile->setPicture( isConstructible ? &grnPicture : &redPicture );
        tile->setMasterTile( 0 );
        tile->setFlag( Tile::clearAll, true );
        tile->setFlag( Tile::tlBuilding, true );
        tile->setOverlay( 0 );
        _d->postTiles.push_back( tile );
      }
    }
  }
}

Tile* CityRenderer::getTile(const TilePos& pos )
{
  return &_d->tilemap->at( pos );
}

Signal1< const Tile& >& CityRenderer::onShowTileInfo()
{
  return _d->onShowTileInfoSignal;
}

void CityRenderer::setMode( const TilemapChangeCommandPtr command )
{
  _d->changeCommand = command;
  _d->startCursorPos = _d->lastCursorPos;
  _d->lmbPressed = false;

  //_d->startCursorPos = Point( -1, -1 );
  updatePreviewTiles();

  if( _d->changeCommand.is<TilemapOverlayCommand>() )
  {
    TilemapOverlayCommandPtr ovCmd = _d->changeCommand.as<TilemapOverlayCommand>();
    switch( ovCmd->getType() )
    {
    case drwWater: _d->setDrawFunction( _d.data(), &Impl::drawTileWater ); break;
    case OV_RISK_FIRE: _d->setDrawFunction( _d.data(), &Impl::drawTileFire ); break;
    case OV_RISK_DAMAGE: _d->setDrawFunction( _d.data(), &Impl::drawTileDamage ); break;
    case OV_COMMERCE_PRESTIGE: _d->setDrawFunction( _d.data(), &Impl::drawTileDesirability ); break;
    case OV_COMMERCE_FOOD: _d->setDrawFunction( _d.data(), &Impl::drawTileFood ); break;
    case OV_RELIGION: _d->setDrawFunction( _d.data(), &Impl::drawTileReligion ); break;
    case OV_ENTERTAINMENT_ALL:
      _d->setDrawFunction( _d.data(), &Impl::drawTileEntertainment );
      _d->overlayRendeFlags.clear();
      _d->overlayRendeFlags.insert( B_MAX );
      _d->overlayRendeFlags.insert( B_THEATER );
      _d->overlayRendeFlags.insert( buildingAmphitheater );
      _d->overlayRendeFlags.insert( B_COLLOSSEUM );
      _d->overlayRendeFlags.insert( B_HIPPODROME );
      _d->overlayRendeFlags.insert( B_ACTOR_COLONY );
      _d->overlayRendeFlags.insert( B_GLADIATOR_SCHOOL );
    break;

    case OV_HEALTH_DOCTOR:
      _d->setDrawFunction( _d.data(), &Impl::drawTileHealth );
      _d->overlayRendeFlags.insert( B_DOCTOR );
    break;

    case OV_HEALTH_HOSPITAL:
      _d->setDrawFunction( _d.data(), &Impl::drawTileHealth );
      _d->overlayRendeFlags.insert( B_HOSPITAL );
    break;

    case OV_HEALTH_BARBER:
      _d->setDrawFunction( _d.data(), &Impl::drawTileHealth );
      _d->overlayRendeFlags.insert( B_BARBER );
    break;

    case OV_HEALTH_BATHS:
      _d->setDrawFunction( _d.data(), &Impl::drawTileHealth );
      _d->overlayRendeFlags.insert( B_BATHS );
    break;

    case OV_ENTERTAINMENT_THEATRES:
      _d->setDrawFunction( _d.data(), &Impl::drawTileEntertainment );
      _d->overlayRendeFlags.clear();
      _d->overlayRendeFlags.insert( B_THEATER );
      _d->overlayRendeFlags.insert( B_ACTOR_COLONY );
    break;

    case OV_ENTERTAINMENT_AMPHITHEATRES:
      _d->setDrawFunction( _d.data(), &Impl::drawTileEntertainment );
      _d->overlayRendeFlags.clear();
      _d->overlayRendeFlags.insert( buildingAmphitheater );
      _d->overlayRendeFlags.insert( B_ACTOR_COLONY );
      _d->overlayRendeFlags.insert( B_GLADIATOR_SCHOOL );
    break;

    case OV_ENTERTAINMENT_COLLISEUM:
      _d->setDrawFunction( _d.data(), &Impl::drawTileEntertainment );
      _d->overlayRendeFlags.clear();
      _d->overlayRendeFlags.insert( B_COLLOSSEUM );
      _d->overlayRendeFlags.insert( B_GLADIATOR_SCHOOL );
    break;

    case OV_ENTERTAINMENT_HIPPODROME:
      _d->setDrawFunction( _d.data(), &Impl::drawTileEntertainment );
      _d->overlayRendeFlags.clear();
      _d->overlayRendeFlags.insert( B_HIPPODROME );
      _d->overlayRendeFlags.insert( B_CHARIOT_MAKER );
    break;

    default:_d->setDrawFunction( _d.data(), &Impl::drawTileBase ); break;
    }

    switch( ovCmd->getType() )
    {
    case drwWater: _d->setVisibleWalkers(WalkersVisibility::nobody); break;
    case OV_RISK_FIRE: _d->setVisibleWalkers(WalkersVisibility::prefect); break;
    case OV_RISK_DAMAGE: _d->setVisibleWalkers(WalkersVisibility::engineer); break;
    case OV_COMMERCE_PRESTIGE: _d->setVisibleWalkers(WalkersVisibility::prestige); break;
    case OV_COMMERCE_FOOD: _d->setVisibleWalkers(WalkersVisibility::food); break;
    case OV_RELIGION: _d->setVisibleWalkers(WalkersVisibility::religion); break;
    case OV_ENTERTAINMENT_ALL: _d->setVisibleWalkers(WalkersVisibility::allEntertainment); break;
    case OV_ENTERTAINMENT_THEATRES: _d->setVisibleWalkers(WalkersVisibility::actorsOnly); break;
    case OV_ENTERTAINMENT_AMPHITHEATRES: _d->setVisibleWalkers(WalkersVisibility::gladiatorsOnly); break;
    case OV_ENTERTAINMENT_COLLISEUM: _d->setVisibleWalkers(WalkersVisibility::tamersOnly); break;
    case OV_ENTERTAINMENT_HIPPODROME: _d->setVisibleWalkers(WalkersVisibility::chariotsOnly); break;
    case OV_HEALTH_DOCTOR: _d->setVisibleWalkers( WalkersVisibility::doctorsOnly ); break;
    case OV_HEALTH_HOSPITAL: _d->setVisibleWalkers( WalkersVisibility::hospitalOnly ); break;
    case OV_HEALTH_BARBER: _d->setVisibleWalkers( WalkersVisibility::barberOnly ); break;
    case OV_HEALTH_BATHS: _d->setVisibleWalkers( WalkersVisibility::bathsOnly ); break;
    default:
      _d->setVisibleWalkers(WalkersVisibility::all);
      break;
    }

    _d->changeCommand = TilemapChangeCommandPtr();
  }
}

void CityRenderer::animate(unsigned int time)
{
  TilemapArea visibleTiles = _d->camera.getTiles();

  foreach( Tile* tile, visibleTiles )
  {
    tile->animate( time );
  }
}

void CityRenderer::setScrollSpeed(int value)
{
  _d->scrollSpeed = value;
}

Signal1< std::string >& CityRenderer::onWarningMessage()
{
  return _d->onWarningMessageSignal;
}

Tilemap& CityRenderer::getTilemap()
{ 
  return *_d->tilemap;
}
