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
#include "layers/sentiment.hpp"
#include "layers/market_access.hpp"
#include "walker/walker.hpp"
#include "objects/aqueduct.hpp"
#include "tilemap_camera.hpp"
#include "city/city.hpp"
#include "game/settings.hpp"
#include "core/timer.hpp"
#include "pathway/pathway.hpp"

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
  int lastZoom;
  Renderer::ModePtr changeCommand;

public:
  LayerPtr currentLayer;
  void setLayer( int type );
  void resetWalkersAfterTurn();
  void saveSettings();

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
  _d->camera.init( *_d->tilemap, engine->virtualSize() );
  _d->engine = engine;
  _d->lastZoom = _d->camera.zoom();
  _d->engine->setScale( 1.f );

  addLayer( Simple::create( _d->camera, city ) );
  addLayer( Water::create( _d->camera, city ) );
  addLayer( Fire::create( _d->camera, city ) );
  addLayer( Food::create( _d->camera, city ) );
  addLayer( Health::create( _d->camera, city, citylayer::health ));
  addLayer( Health::create( _d->camera, city, citylayer::doctor ));
  addLayer( Health::create( _d->camera, city, citylayer::hospital ));
  addLayer( Health::create( _d->camera, city, citylayer::barber ));
  addLayer( Health::create( _d->camera, city, citylayer::baths ));
  addLayer( Religion::create( _d->camera, city ) );
  addLayer( Damage::create( _d->camera, city ) );
  addLayer( Sentiment::create( _d->camera, city ) );
  addLayer( Unemployed::create( _d->camera, city ) );
  addLayer( citylayer::Desirability::create( _d->camera, city ) );
  addLayer( Entertainment::create( _d->camera, city, citylayer::entertainment ) );
  addLayer( Entertainment::create( _d->camera, city, citylayer::theater ) );
  addLayer( Entertainment::create( _d->camera, city, citylayer::amphitheater ) );
  addLayer( Entertainment::create( _d->camera, city, citylayer::colloseum ) );
  addLayer( Entertainment::create( _d->camera, city, citylayer::hippodrome ) );
  addLayer( Crime::create( _d->camera, city ) ) ;
  addLayer( Build::create( *this, city ) );
  addLayer( Destroy::create( *this, city ) );
  addLayer( Tax::create( _d->camera, city ) );
  addLayer( Education::create( _d->camera, city, citylayer::education ) );
  addLayer( Education::create( _d->camera, city, citylayer::school ) );
  addLayer( Education::create( _d->camera, city, citylayer::library ) );
  addLayer( Education::create( _d->camera, city, citylayer::academy ) );
  addLayer( Troubles::create( _d->camera, city, citylayer::risks ) );
  addLayer( Troubles::create( _d->camera, city, citylayer::troubles ) );
  addLayer( Aborigens::create( _d->camera, city ) );
  addLayer( MarketAccess::create( _d->camera, city ) );

  DrawOptions& dopts = DrawOptions::instance();
  dopts.setFlag( DrawOptions::borderMoving, engine->isFullscreen() );
  dopts.setFlag( DrawOptions::windowActive, true );
  dopts.setFlag( DrawOptions::mayChangeLayer, true );
  dopts.setFlag( DrawOptions::oldGraphics, oldGraphic );
  dopts.setFlag( DrawOptions::showBuildings, true );
  dopts.setFlag( DrawOptions::showTrees, true );
  dopts.setFlag( DrawOptions::mmbMoving, SETTINGS_VALUE( mmb_moving ) );

  _d->setLayer( citylayer::simple );
}

void CityRenderer::Impl::resetWalkersAfterTurn()
{
  const WalkerList& walkers = city->walkers();

  foreach( it, walkers )
  {
    WalkerPtr w = *it;
    w->setPos( w->tile().epos() );
  }
}

void CityRenderer::Impl::saveSettings()
{
  DrawOptions& dopts = DrawOptions::instance();
  SETTINGS_SET_VALUE( mmb_moving, dopts.isFlag( DrawOptions::mmbMoving ) );
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
  foreach( it, layers )
  {
    if( (*it)->type() == type )
    {
      newLayer = *it;
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

  if( _d->lastZoom != _d->camera.zoom() )
  {
    _d->lastZoom = _d->camera.zoom();

    float fzoom = _d->lastZoom / 100.f;
    Size s = engine.screenSize() * (1/fzoom);
    engine.setScale( fzoom );
    _d->camera.setViewport( s );
  }

  if( _d->city->getOption( PlayerCity::updateTiles ) > 0 )
  {
    _d->camera.refresh();
    _d->city->setOption( PlayerCity::updateTiles, 0 );
  }

  if( layer.isNull() )
  {
    return;
  }

  engine.setScale( _d->lastZoom / 100.f );

  layer->beforeRender( engine );
  layer->render( engine );
  layer->afterRender( engine );

  engine.setScale( 1.f );

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

  foreach( i, visibleTiles )
  {
    (*i)->animate( time );
  }
}

void CityRenderer::rotateRight()
{
  _d->tilemap->turnRight();
  _d->camera.refresh();
  _d->camera.tiles();
  _d->resetWalkersAfterTurn();  
}

void CityRenderer::rotateLeft()
{
  _d->tilemap->turnLeft();
  _d->camera.refresh();
  _d->camera.tiles();
  _d->resetWalkersAfterTurn();
}

void CityRenderer::setLayer(int layertype)
{
  if( _d->currentLayer->type() == layertype )
    layertype = citylayer::simple;

  _d->setLayer( layertype );
}

LayerPtr CityRenderer::getLayer(int type) const
{
  foreach( it, _d->layers)
  {
    if( (*it)->type() == type )
      return *it;
  }

  return LayerPtr();
}

TilePos CityRenderer::screen2tilepos(const Point& point ) const{  return _d->camera.at( point, true )->pos();}

Camera* CityRenderer::camera() {  return &_d->camera; }
Renderer::ModePtr CityRenderer::mode() const {  return _d->changeCommand;}
void CityRenderer::addLayer( LayerPtr layer){  _d->layers.push_back( layer ); }
LayerPtr CityRenderer::currentLayer() const { return _d->currentLayer; }
void CityRenderer::setViewport(const Size& size) { _d->camera.setViewport( size ); }
Signal1<int>& CityRenderer::onLayerSwitch() { return _d->onLayerSwitchSignal; }

}//end namespace gfx
