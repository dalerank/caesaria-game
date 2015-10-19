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

#include "commodity_turnover.hpp"
#include "objects/constants.hpp"
#include "objects/house.hpp"
#include "objects/house_spec.hpp"
#include "game/resourcegroup.hpp"
#include "constants.hpp"
#include "core/logger.hpp"
#include "city/statistic.hpp"
#include "core/event.hpp"
#include "good/turnover.hpp"
#include "good/store.hpp"
#include "game/gamedate.hpp"
#include "gfx/tilemap_camera.hpp"

using namespace gfx;

namespace citylayer
{

class CommodityTurnover::Impl
{
public:
  struct {
    OverlayPtr selected;
    OverlayPtr current;
  } overlay;

  DateTime lastUpdate;
  PointsArray points;
  std::set<const Tile*> peersArea;
  std::vector<TilesArray> ways;

public:  
  bool initContiditon( BuildingPtr b1, BuildingPtr b2 );
  void canAppend(const gfx::Tile* tile, bool& ret);
};

int CommodityTurnover::type() const {  return citylayer::comturnover; }

void CommodityTurnover::drawTile(const RenderInfo& rinfo, Tile& tile)
{
/*
  if( tile.overlay().isNull() )
  {
    drawPass( rinfo, tile, Renderer::ground );
    drawPass( rinfo, tile, Renderer::groundAnimation );
  }
  else
  {
    bool needDrawAnimations = false;
    OverlayPtr overlay = tile.overlay();
    int accessLevel = 0;

    if( _isVisibleObject( overlay->type() ) )
    {
      needDrawAnimations = true;
    }
    else if( overlay->type() == object::house )
    {
      auto house = overlay.as<House>();
      accessLevel = (int)house->getServiceValue( Service::market );
      needDrawAnimations = (house->level() <= HouseLevel::hovel) && house->habitants().empty();

      if( !needDrawAnimations )
      {
        drawArea( rinfo, overlay->area(), ResourceGroup::foodOverlay, config::id.overlay.inHouseBase );
      }
    }
    else
    {
      drawArea( rinfo, overlay->area(), ResourceGroup::foodOverlay, config::id.overlay.base );
    }

    if( needDrawAnimations )
    {
      Layer::drawTile( rinfo, tile );
      registerTileForRendering( tile );
    }
    else if( accessLevel >= 0 )
    {
      Point screenPos = tile.mappos() + rinfo.offset;
      drawColumn( rinfo, screenPos, accessLevel );
    }
  }

  tile.setRendered();
*/
  Layer::drawTile(rinfo, tile);
}

void CommodityTurnover::afterRender(Engine& engine)
{
  Info::afterRender( engine );

  if( _d->lastUpdate.daysTo( game::Date::current() ) > 1 )
  {
    _d->lastUpdate = game::Date::current();
    _updateStorePath();
  }
}

void CommodityTurnover::render(Engine& engine)
{
  Info::render( engine );

  RenderInfo rinfo{ engine, _camera()->offset() };
  _renderPaths( rinfo );
}

void CommodityTurnover::Impl::canAppend(const gfx::Tile* tile, bool& ret)
{
  ret = false;
  if( tile->getFlag( Tile::tlRoad ) )
  {
    ret = true;
  }
  else
  {
    ret = peersArea.count( tile ) > 0;
  }
}

void CommodityTurnover::_updateStorePath()
{
  BuildingPtr building = _d->overlay.selected.as<Building>();
  _d->ways.clear();
  if( building.isValid() )
  {
    good::Store& store = building->store();
    good::Turnovers consumers = store.consumers().items();

    for( auto& item : consumers )
    {
      bool isOk = _d->initContiditon( building, _city()->getOverlay( item.receiver ).as<Building>() );

      if( isOk )
      {
        Pathway pathway = PathwayHelper::create( item.sender, item.receiver, makeDelegate( _d.data(), &Impl::canAppend ) );
        if( pathway.isValid() )
        {
          _d->ways.push_back( pathway.allTiles() );
        }
        else
        {
          isOk = false;
        }
      }

      if( !isOk )
      {
        OverlayPtr b1 = _city()->getOverlay( item.sender );
        OverlayPtr b2 = _city()->getOverlay( item.receiver );
        Logger::warning( "CommodityTurnover: cant create path from [{},{}]:{} to [{},{}]:{}",
                         item.sender.i(), item.sender.j(), b1.isValid() ? b1->info().name() : "unknown",
                         item.receiver.i(), item.receiver.j(), b2.isValid() ? b2->info().name() : "unknown" );
      }
    }
  }
}

bool CommodityTurnover::Impl::initContiditon(BuildingPtr b1, BuildingPtr b2)
{
  if( b1.isNull() || b2.isNull() )
  {
    return false;
  }

  peersArea.clear();
  for( auto tile : b1->area() )
    peersArea.insert( tile );

  for( auto tile : b2->area() )
    peersArea.insert( tile );

  return true;
}

LayerPtr CommodityTurnover::create( Camera& camera, PlayerCityPtr city)
{
  LayerPtr ret( new CommodityTurnover( camera, city ) );
  ret->drop();

  return ret;
}

void CommodityTurnover::_renderPaths(const RenderInfo& rinfo)
{
  for( auto& tiles : _d->ways )
  {
    PointsArray points = tiles.mappositions();
    points.move( rinfo.offset + Point( gfx::tilemap::cellPicSize().width() / 2, 0 ) );
    rinfo.engine.drawLines( DefaultColors::red, points );
  }
}

void CommodityTurnover::handleEvent(NEvent& event)
{
  if( event.EventType == sEventMouse )
  {    
    switch( event.mouse.type  )
    {
    case mouseMoved:
    {
      Tile* tile = _camera()->at( event.mouse.pos(), false );  // tile under the cursor (or NULL)
      if( tile != 0 )
        _d->overlay.current = tile->overlay();
    }
    break;

    case mouseLbtnPressed:
    {
      _d->overlay.selected = _d->overlay.current;
    }
    break;

    default: break;
    }
  }

  Layer::handleEvent( event );
}

CommodityTurnover::CommodityTurnover( Camera& camera, PlayerCityPtr city)
  : Info( camera, city, 0 ), _d( new Impl )
{
  _addWalkerType( walker::cartPusher );
  _addWalkerType( walker::supplier );
  _addWalkerType( walker::marketBuyer );
  _addWalkerType( walker::marketLady );
  _addWalkerType( walker::marketKid );

  _initialize();
}

}//end namespace city
