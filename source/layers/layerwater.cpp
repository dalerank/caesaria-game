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
// Copyright 2012-2015 Dalerank, dalerankn8@gmail.com

#include "layerwater.hpp"
#include "objects/constants.hpp"
#include "game/resourcegroup.hpp"
#include "objects/house.hpp"
#include "objects/house_spec.hpp"
#include "constants.hpp"
#include "city/statistic.hpp"
#include "core/color_list.hpp"
#include "gfx/tilemap.hpp"
#include "core/event.hpp"
#include "objects/watersupply.hpp"
#include "objects/fountain.hpp"
#include "core/gettext.hpp"
#include "gfx/tilemap_camera.hpp"
#include "objects/aqueduct.hpp"
#include "gfx/textured_path.hpp"
#include "game/gamedate.hpp"
#include "core/font.hpp"
#include "core/utils.hpp"

using namespace gfx;

namespace citylayer
{

class Water::Impl
{
public:
  std::set<int> flags;
  bool showWaterValue;
  std::map<int, Picture> pics;

  struct
  {
    OverlayPtr selected;
    OverlayPtr underMouse;
  } overlay;

  std::vector<ColoredWay> ways;
};

int Water::type() const{  return citylayer::water;}

void Water::drawTile( const RenderInfo& rinfo, Tile& tile)
{
  bool needDrawAnimations = false;
  Size areaSize(1);

  if( tile.overlay().isNull() )
  {
    drawLandTile( rinfo, tile );
  }
  else
  {
    OverlayPtr overlay = tile.overlay();
    if( _isVisibleObject( overlay->type() ) )
    {
      // Base set of visible objects
      needDrawAnimations = true;
      areaSize = overlay->size();      
    }
    else
    {
      int tileNumber = 0;
      bool haveWater = tile.param( Tile::pFountainWater ) > 0 || tile.param( Tile::pWellWater ) > 0;
      needDrawAnimations = false;

      if ( overlay->type() == object::house )
      {
        auto house = overlay.as<House>();
        needDrawAnimations = (house->level() <= HouseLevel::hovel) && house->habitants().empty();

        tileNumber = config::tile.house;
        haveWater = haveWater || house->hasServiceAccess(Service::fountain) || house->hasServiceAccess(Service::well);
      }

      if( !needDrawAnimations )
      {
        tileNumber += (haveWater ? config::layer.haveWater : 0);
        tileNumber += tile.param( Tile::pReservoirWater ) > 0 ? config::layer.reservoirRange : 0;

        drawArea( rinfo, overlay->area(), config::layer.water, config::tile.constr + tileNumber );

        areaSize = Size( 0 );
      }
    }

    if ( needDrawAnimations )
    {
      Point screenPos = tile.mappos() + rinfo.offset;
      Layer::drawTile( rinfo, tile );

      if( _d->showWaterValue )
      {
        auto aqueduct = tile.overlay<Aqueduct>();
        if( aqueduct.isValid() )
        {
          Font f = Font::create( FONT_2 ).withColor( 0xffff0000 );
          int df = aqueduct->water();
          f.draw( rinfo.engine.screen(), utils::format( 0xff, "%x", df), screenPos + Point( 20, -80 ), false );
        }

        int wellValue = tile.param( Tile::pWellWater );
        int fountainValue = tile.param( Tile::pFountainWater );
        int reservoirWater = tile.param( Tile::pReservoirWater );

        if( wellValue > 0 || fountainValue > 0 || reservoirWater > 0 )
        {
          std::string text = utils::format( 0xff, "%d/%d/%d", wellValue, fountainValue, reservoirWater );
          Font f = Font::create( FONT_2 ).withColor( 0xffff0000 );
          f.draw( rinfo.engine.screen(), text, screenPos + Point( 20, -80 ), false );
        }
      }
      registerTileForRendering( tile );
    }
  }

  if( !needDrawAnimations && ( tile.isWalkable(true) || tile.getFlag( Tile::tlOverlay ) ) )
  {
    _drawLandTile( rinfo, tile, areaSize );
  }

  tile.setRendered();
}

void Water::_drawLandTile(const RenderInfo& rinfo, Tile& tile, const Size& areaSize )
{
  Tilemap& tilemap = _city()->tilemap();
  TilesArray area = tilemap.area( tile.epos(), areaSize );

  for( auto rtile : area )
  {
    int reservoirWater = rtile->param( Tile::pReservoirWater );
    int fontainWater = rtile->param( Tile::pFountainWater );

    if( (reservoirWater + fontainWater > 0) && ! rtile->getFlag( Tile::tlWater ) && rtile->overlay().isNull() )
    {
      int picIndex = reservoirWater ? config::layer.reservoirRange : 0;
      picIndex |= fontainWater > 0 ? config::layer.haveWater : 0;
      picIndex |= config::tile.skipLeftBorder | config::tile.skipRightBorder;
      rinfo.engine.draw( _d->pics[ picIndex + config::tile.constr ], rtile->mappos() + rinfo.offset );
    }
  }
}

void Water::drawPass( const RenderInfo& rinfo, Tile& tile, Renderer::Pass pass)
{
  if( pass == Renderer::groundAnimation )
    _drawLandTile( rinfo, tile, Size( 1 ) );
  else
    Layer::drawPass( rinfo, tile, pass );
}

void Water::drawWalkerOverlap( const RenderInfo& rinfo, Tile& tile, const int depth)
{

}

void Water::handleEvent(NEvent& event)
{
  if( event.EventType == sEventKeyboard )
  {
    if( event.keyboard.control && !event.keyboard.pressed && event.keyboard.key == KEY_KEY_E )
    {
      _d->showWaterValue = !_d->showWaterValue;
    }
  }

  if( event.EventType == sEventMouse )
  {
    switch( event.mouse.type  )
    {
    case mouseMoved:
    {
      Tile* tile = _camera()->at( event.mouse.pos(), false );  // tile under the cursor (or NULL)
      std::string text = "";
      if( tile != 0 )
      {
        bool isWater = tile->getFlag( Tile::tlWater ) || tile->getFlag( Tile::tlDeepWater );
        if( !isWater )
        {
          int wtrSrvc = (tile->param( Tile::pWellWater ) > 0 ? 1 : 0);
          wtrSrvc |= (tile->param( Tile::pFountainWater ) > 0 ? 2 : 0);
          wtrSrvc |= (tile->param( Tile::pReservoirWater ) > 0 ? 4 : 0);

          switch( wtrSrvc )
          {
          case 0:         /*text = "##water_srvc_no_water##";*/ break;
          case 1:         text = "##water_srvc_well##"; break;
          case 2: case 6: text = "##water_srvc_fountain##"; break;
          case 3:         text = "##water_srvc_fountain_and_well##"; break;
          case 7:         text = "##water_srvc_fountain_and_reservoir##"; break;
          case 4: case 5: text = "##water_srvc_reservoir##"; break;
          }
        }

        _d->overlay.underMouse = tile->overlay();
      }

      _setTooltipText( _(text) );
    }
    break;

    case mouseLbtnPressed:
    {
      _d->overlay.selected = _d->overlay.underMouse;
      _updatePaths();
    }
    break;

    default: break;
    }
  }

  Layer::handleEvent( event );
}

void Water::_updatePaths()
{
  auto reservoir = _d->overlay.selected.as<Reservoir>();
  if( reservoir.isValid() )
  {
    _d->ways.clear();
    PathwayCondition wayCondition;
    auto fountains = reservoir->aquifer().overlays<Fountain>();

    for( auto fountain : fountains )
    {
      Pathway pathway = PathwayHelper::create( reservoir->pos(), fountain->pos(),
                                               wayCondition.bySomething(), Pathway::fourDirection  );
      if( pathway.isValid() )
        _d->ways.push_back( { pathway.allTiles(), ColorList::red } );
    }
  }

  auto fountain = _d->overlay.underMouse.as<Fountain>();
  if( fountain.isValid() )
  {
    _d->ways.clear();

    PathwayCondition wayCondition;
    auto reservoirs = _map().rect( 10, fountain->pos() ).overlays<Reservoir>();

    for( auto reservoir : reservoirs )
    {
      Pathway pathway = PathwayHelper::create( reservoir->pos(), fountain->pos(), wayCondition.bySomething() );
      if( pathway.isValid() )
        _d->ways.push_back( { pathway.allTiles(), ColorList::blue } );
    }
  }
}

void Water::render(Engine& engine)
{
  Layer::render( engine );

  RenderInfo rinfo{ engine, _camera()->offset() };
  for( auto& wayinfo : _d->ways )
  {
    TexturedPath::draw( wayinfo.tiles, rinfo, wayinfo.color );
  }
}

void Water::afterRender(Engine& engine)
{
  Layer::afterRender(engine);

  if( game::Date::isDayChanged() )
    _updatePaths();
}

Water::Water( Camera& camera, PlayerCityPtr city)
  : Layer( &camera, city ), _d( new Impl )
{
  _d->showWaterValue = false;
  _initialize();

  for( int i=1; i < 32; i++ )
    _d->pics[ i ] = Picture( config::layer.water, i );
}

}//end namespace citylayer
