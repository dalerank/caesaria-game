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

#include "city_renderer.hpp"

#include "tile.hpp"
#include "engine.hpp"
#include "game/resourcegroup.hpp"
#include "core/position.hpp"
#include "pictureconverter.hpp"
#include "core/event.hpp"
#include "gfx/renderermode.hpp"
#include "gfx/tilemap.hpp"
#include "core/utils.hpp"
#include "objects/house_level.hpp"
#include "core/foreach.hpp"
#include "events/event.hpp"
#include "core/font.hpp"
#include "gfx/sdl_engine.hpp"
#include "core/gettext.hpp"
#include "core/logger.hpp"
#include "layers/layersimple.hpp"
#include "layers/layerwater.hpp"
#include "layers/layerfire.hpp"
#include "layers/layerfood.hpp"
#include "layers/layerhealth.hpp"
#include "layers/constants.hpp"
#include "layers/layerreligion.hpp"
#include "layers/build.hpp"
#include "layers/damage.hpp"
#include "layers/desirability.hpp"
#include "layers/layerentertainment.hpp"
#include "layers/layertax.hpp"
#include "layers/crime.hpp"
#include "layers/layerdestroy.hpp"
#include "layers/layertroubles.hpp"
#include "layers/aborigens.hpp"
#include "layers/layereducation.hpp"
#include "layers/unemployed.hpp"
#include "layers/constructor.hpp"
#include "layers/sentiment.hpp"
#include "layers/market_access.hpp"
#include "layers/commodity_turnover.hpp"
#include "walker/walker.hpp"
#include "objects/aqueduct.hpp"
#include "tilemap_camera.hpp"
#include "city/city.hpp"
#include "game/settings.hpp"
#include "core/timer.hpp"
#include "pathway/pathway.hpp"
#include "gui/environment.hpp"
#include "gui/dialogbox.hpp"

using namespace citylayer;

namespace gfx
{

enum { defaultZoomStep=10 };

class CityRenderer::Impl
{
public: 
  typedef std::vector<LayerPtr> Layers;

  PlayerCityPtr city;     // city to display
  Tilemap* tilemap;
  gui::Ui* guienv;
  Engine* engine;
  TilemapCamera camera;  // visible map area
  Layers layers;
  Point currentCursorPos;
  Renderer::ModePtr changeCommand;
  LayerPtr currentLayer;

public:
  void setLayer( int type );
  void resetWalkersAfterTurn();
  void saveSettings();
  void awareExperimental();

  template<typename Class, typename... Args>
  LayerPtr instanceLayer( Args ... args)
  {
    LayerPtr layer( new Class( camera, city, args... ) );
    layer->drop();
    layers.push_back( layer );

    return layer;
  }

public signals:
  Signal1<int> onLayerSwitchSignal;
};

CityRenderer::CityRenderer() : _d( new Impl )
{
}

CityRenderer::~CityRenderer()
{
  _d->saveSettings();
}

void CityRenderer::initialize(PlayerCityPtr city, Engine* engine, gui::Ui* guienv, bool oldGraphic )
{
  _d->city = city;
  _d->tilemap = &city->tilemap();
  _d->guienv = guienv;
  _d->camera.init( *_d->tilemap, engine->viewportSize() );
  _d->engine = engine;

  _d->instanceLayer<Simple>();
  _d->instanceLayer<Water>();
  _d->instanceLayer<Fire>();
  _d->instanceLayer<Food>();
  _d->instanceLayer<Health>( citylayer::health );
  _d->instanceLayer<Health>( citylayer::doctor );
  _d->instanceLayer<Health>( citylayer::hospital );
  _d->instanceLayer<Health>( citylayer::barber );
  _d->instanceLayer<Health>( citylayer::baths );
  _d->instanceLayer<Religion>();
  _d->instanceLayer<Damage>();
  _d->instanceLayer<Sentiment>();
  _d->instanceLayer<Unemployed>();
  _d->instanceLayer<citylayer::Desirability>();
  _d->instanceLayer<Entertainment>( citylayer::entertainment );
  _d->instanceLayer<Entertainment>( citylayer::theater );
  _d->instanceLayer<Entertainment>( citylayer::amphitheater );
  _d->instanceLayer<Entertainment>( citylayer::colloseum );
  _d->instanceLayer<Entertainment>( citylayer::hippodrome );
  _d->instanceLayer<Crime>();
  addLayer( Destroy::create( *this, city ) );
  _d->instanceLayer<Tax>();
  _d->instanceLayer<Education>( citylayer::education );
  _d->instanceLayer<Education>( citylayer::school );
  _d->instanceLayer<Education>( citylayer::library );
  _d->instanceLayer<Education>( citylayer::academy );
  _d->instanceLayer<Troubles>( citylayer::risks );
  _d->instanceLayer<Troubles>( citylayer::troubles );
  _d->instanceLayer<Aborigens>();
  _d->instanceLayer<MarketAccess>();
  _d->instanceLayer<CommodityTurnover>();
  _d->instanceLayer<Build>( this );
  addLayer( Constructor::create( *this, city ) );

  DrawOptions& dopts = DrawOptions::instance();
  dopts.setFlag( DrawOptions::borderMoving, engine->isFullscreen() );
  dopts.setFlag( DrawOptions::windowActive, true );
  dopts.setFlag( DrawOptions::mayChangeLayer, true );
  dopts.setFlag( DrawOptions::oldGraphics, oldGraphic );
  dopts.setFlag( DrawOptions::showBuildings, true );
  dopts.setFlag( DrawOptions::showTrees, true );
  dopts.setFlag( DrawOptions::mmbMoving, KILLSWITCH( mmb_moving ) );
  dopts.setFlag( DrawOptions::overdrawOnBuild, false );
  dopts.setFlag( DrawOptions::rotateEnabled, false );

#ifdef DEBUG
  dopts.setFlag( DrawOptions::rotateEnabled, true );
#endif

  _d->setLayer( citylayer::simple );
}

void CityRenderer::Impl::resetWalkersAfterTurn()
{
  const WalkerList& walkers = city->walkers();

  for( auto wlk : walkers )
    wlk->mapTurned();
}

void CityRenderer::Impl::saveSettings()
{
  DrawOptions& dopts = DrawOptions::instance();
  SETTINGS_SET_VALUE( mmb_moving, dopts.isFlag( DrawOptions::mmbMoving ) );
}

void CityRenderer::Impl::awareExperimental()
{
#ifdef DEBUG
  return;
#endif
  if( city->tilemap().direction() != direction::north )
  {
    auto dlg = gui::dialog::Information( guienv, "Note", "Sorry, rotated map yet expiremental mode\ngame may work incorrect." );
    dlg->show();
  }
}

void CityRenderer::Impl::setLayer(int type)
{
  if( currentLayer.isValid() )
  {
    currentLayer->changeLayer( type );
  }

  if( !DrawOptions::instance().isFlag( DrawOptions::mayChangeLayer ) )
  {
    return;
  }

  LayerPtr newLayer;
  for( auto layer : layers )
  {
    if( layer->type() == type )
    {
      newLayer = layer;
      break;
    }
  }

  if( newLayer.isNull() )
  {
    newLayer = layers.front();
  }

  newLayer->init( currentCursorPos );
  currentLayer = newLayer;
  emit onLayerSwitchSignal( currentLayer->type() );
}

void CityRenderer::render()
{  
  LayerPtr layer = _d->currentLayer;
  Engine& engine = *_d->engine;
  static int lastZoom = 0;

  if( _d->city->getOption( PlayerCity::updateTiles ) > 0 )
  {
    _d->camera.refresh();
    _d->city->setOption( PlayerCity::updateTiles, 0 );
  }

  if( layer.isNull() )
  {
    return;
  }

  float zoom;
  if( lastZoom != _d->camera.zoom() )
  {
    lastZoom = _d->camera.zoom();
    zoom = lastZoom / 100.f;
    _d->camera.setViewport( engine.viewportSize()  / zoom );
  }

  zoom = lastZoom / 100.f;

  engine.setScale( zoom );

  layer->beforeRender( engine );
  layer->render( engine );
  layer->afterRender( engine );

  engine.setVirtualSize( _d->guienv->vsize() );

  layer->renderUi( engine );

  if( layer->type() != layer->nextLayer() )
  {
    _d->setLayer( _d->currentLayer->nextLayer() );
  }
}

void CityRenderer::handleEvent( NEvent& event )
{
  if( event.EventType == sEventMouse )
  {
    _d->currentCursorPos = event.mouse.pos();

    if( event.mouse.type == mouseWheel )
    {
      if( _d->city->getOption( PlayerCity::zoomEnabled ) )
      {
        int zoomInvert = _d->city->getOption( PlayerCity::zoomInvert ) ? -1 : 1;

        _d->camera.changeZoom( event.mouse.wheel * defaultZoomStep * zoomInvert );
      }
    }
  }

  if( _d->currentLayer.isValid() )
    _d->currentLayer->handleEvent( event );
}

int CityRenderer::layerType() const
{
  return _d->currentLayer->type();
}

void CityRenderer::setMode( Renderer::ModePtr command )
{
  _d->changeCommand = command;

  LayerModePtr ovCmd = _d->changeCommand.as<LayerMode>();
  if( ovCmd.isValid() )
  {
    _d->setLayer( ovCmd->type() );
  }
}

void CityRenderer::animate(unsigned int time)
{
  const TilesArray& visibleTiles = _d->camera.tiles();

  for( auto&& tile : visibleTiles )
    tile->animate( time );
}

void CityRenderer::rotateRight()
{
  _d->tilemap->turnRight();
  _d->camera.refresh();
  _d->camera.tiles();
  _d->resetWalkersAfterTurn();  
  _d->awareExperimental();
}

void CityRenderer::rotateLeft()
{
  _d->tilemap->turnLeft();
  _d->camera.refresh();
  _d->camera.tiles();
  _d->resetWalkersAfterTurn();
  _d->awareExperimental();
}

void CityRenderer::setLayer(int layertype)
{
  if( _d->currentLayer->type() == layertype )
    layertype = citylayer::simple;

  _d->setLayer( layertype );
}

LayerPtr CityRenderer::getLayer(int type) const
{
  for( auto& layer : _d->layers)
  {
    if( layer->type() == type )
      return layer;
  }

  return LayerPtr();
}

TilePos CityRenderer::screen2tilepos(const Point& point ) const{  return _d->camera.at( point, true )->pos();}

Camera* CityRenderer::camera() {  return &_d->camera; }
Renderer::ModePtr CityRenderer::mode() const {  return _d->changeCommand;}
void CityRenderer::addLayer(SmartPtr<Layer> layer){  _d->layers.push_back( layer ); }
LayerPtr CityRenderer::currentLayer() const { return _d->currentLayer; }
void CityRenderer::setViewport(const Size& size) { _d->camera.setViewport( size ); }
Signal1<int>& CityRenderer::onLayerSwitch() { return _d->onLayerSwitchSignal; }

Signal3<object::Type,TilePos,int>& CityRenderer::onBuilt()
{
  auto buildLayer = getLayer( citylayer::build ).as<Build>();
  return buildLayer->onBuild();
}

Signal3<object::Type,TilePos,int>& CityRenderer::onDestroyed()
{
  auto buildLayer = getLayer( citylayer::destroyd ).as<Destroy>();
  return buildLayer->onDestroy();
}


}//end namespace gfx
