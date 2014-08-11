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

#include "city_renderer.hpp"

#include "tile.hpp"
#include "engine.hpp"
#include "game/resourcegroup.hpp"
#include "core/position.hpp"
#include "pictureconverter.hpp"
#include "core/event.hpp"
#include "gfx/renderermode.hpp"
#include "gfx/tilemap.hpp"
#include "core/stringhelper.hpp"
#include "objects/house_level.hpp"
#include "core/foreach.hpp"
#include "events/event.hpp"
#include "core/font.hpp"
#include "gfx/sdl_engine.hpp"
#include "core/gettext.hpp"
#include "core/logger.hpp"
#include "layersimple.hpp"
#include "layerwater.hpp"
#include "layerfire.hpp"
#include "layerfood.hpp"
#include "layerhealth.hpp"
#include "layerconstants.hpp"
#include "layerreligion.hpp"
#include "layerbuild.hpp"
#include "layerdamage.hpp"
#include "layerdesirability.hpp"
#include "layerentertainment.hpp"
#include "layertax.hpp"
#include "layercrime.hpp"
#include "walker/walker.hpp"
#include "objects/aqueduct.hpp"
#include "layerdestroy.hpp"
#include "tilemap_camera.hpp"
#include "layereducation.hpp"
#include "city/city.hpp"
#include "layertroubles.hpp"
#include "layerindigene.hpp"

using namespace constants;

namespace gfx
{

class CityRenderer::Impl
{
public: 
  typedef std::vector<LayerPtr> Layers;

  PlayerCityPtr city;     // city to display
  Tilemap* tilemap;
  gui::GuiEnv* guienv;
  Engine* engine;
  TilemapCamera camera;  // visible map area
  Layers layers;
  Point currentCursorPos;

  Renderer::ModePtr changeCommand;

  LayerPtr currentLayer;
  void setLayer( int type );

public oc3_signals:
  Signal1<int> onLayerSwitchSignal;
};

CityRenderer::CityRenderer() : _d( new Impl )
{
}

CityRenderer::~CityRenderer() {}

void CityRenderer::initialize(PlayerCityPtr city, Engine* engine, gui::GuiEnv* guienv )
{
  _d->city = city;
  _d->tilemap = &city->tilemap();
  _d->guienv = guienv;
  _d->camera.init( *_d->tilemap );
  _d->engine = engine;

  addLayer( LayerSimple::create( _d->camera, city ) );
  addLayer( LayerWater::create( _d->camera, city ) );
  addLayer( LayerFire::create( _d->camera, city ) );
  addLayer( LayerFood::create( _d->camera, city ) );
  addLayer( LayerHealth::create( _d->camera, city, citylayer::health ));
  addLayer( LayerHealth::create( _d->camera, city, citylayer::doctor ));
  addLayer( LayerHealth::create( _d->camera, city, citylayer::hospital ));
  addLayer( LayerHealth::create( _d->camera, city, citylayer::barber ));
  addLayer( LayerHealth::create( _d->camera, city, citylayer::baths ));
  addLayer( LayerReligion::create( _d->camera, city ) );
  addLayer( LayerDamage::create( _d->camera, city ) );
  addLayer( LayerDesirability::create( _d->camera, city ) );
  addLayer( LayerEntertainment::create( _d->camera, city, citylayer::entertainment ) );
  addLayer( LayerEntertainment::create( _d->camera, city, citylayer::theater ) );
  addLayer( LayerEntertainment::create( _d->camera, city, citylayer::amphitheater ) );
  addLayer( LayerEntertainment::create( _d->camera, city, citylayer::colloseum ) );
  addLayer( LayerEntertainment::create( _d->camera, city, citylayer::hippodrome ) );
  addLayer( LayerCrime::create( _d->camera, city ) ) ;
  addLayer( LayerBuild::create( this, city ) );
  addLayer( LayerDestroy::create( _d->camera, city ) );
  addLayer( LayerTax::create( _d->camera, city ) );
  addLayer( LayerEducation::create( _d->camera, city, citylayer::education ) );
  addLayer( LayerEducation::create( _d->camera, city, citylayer::school ) );
  addLayer( LayerEducation::create( _d->camera, city, citylayer::library ) );
  addLayer( LayerEducation::create( _d->camera, city, citylayer::academy ) );
  addLayer( LayerTroubles::create( _d->camera, city, citylayer::risks ) );
  addLayer( LayerTroubles::create( _d->camera, city, citylayer::troubles ) );
  addLayer( LayerIndigene::create( _d->camera, city ) );

  _d->setLayer( citylayer::simple );
}

void CityRenderer::Impl::setLayer(int type)
{
  currentLayer = 0;
  foreach( layer, layers )
  {
    if( (*layer)->type() == type )
    {
      currentLayer = *layer;
      break;
    }
  }

  if( currentLayer.isNull() )
  {
    currentLayer = layers.front();
  }

  currentLayer->init( currentCursorPos );
  oc3_emit onLayerSwitchSignal( currentLayer->type() );
}

void CityRenderer::render()
{
  if( _d->currentLayer.isNull() )
  {
    return;
  }

  _d->currentLayer->beforeRender( *_d->engine );

  _d->currentLayer->render( *_d->engine );

  _d->currentLayer->renderPass( *_d->engine, Renderer::animations );

  _d->currentLayer->afterRender( *_d->engine );

  if( _d->currentLayer->type() != _d->currentLayer->nextLayer() )
  {
    _d->setLayer( _d->currentLayer->nextLayer() );
  }
}

void CityRenderer::handleEvent( NEvent& event )
{
  if( event.EventType == sEventMouse )
  {
    _d->currentCursorPos = event.mouse.pos();
  }

  _d->currentLayer->handleEvent( event );
}

int CityRenderer::layerType() const
{
  return _d->currentLayer->type();
}

void CityRenderer::setMode( Renderer::ModePtr command )
{
  _d->changeCommand = command;

  LayerModePtr ovCmd = ptr_cast<LayerMode>( _d->changeCommand );
  if( ovCmd.isValid() )
  {
    _d->setLayer( ovCmd->getType() );
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
}

void CityRenderer::rotateLeft()
{
  _d->tilemap->turnLeft();
  _d->camera.refresh();
  _d->camera.tiles();
}

Camera* CityRenderer::camera() {  return &_d->camera; }
Renderer::ModePtr CityRenderer::getMode() const {  return _d->changeCommand;}
void CityRenderer::addLayer(LayerPtr layer){  _d->layers.push_back( layer ); }
void CityRenderer::setLayer(int layertype) { _d->setLayer( layertype ); }
TilePos CityRenderer::getTilePos( Point point ) const{  return _d->camera.at( point, true )->pos();}
void CityRenderer::setViewport(const Size& size){ _d->camera.setViewport( size ); }
Signal1<int>&CityRenderer::onLayerSwitch() { return _d->onLayerSwitchSignal; }
Tilemap& CityRenderer::getTilemap(){   return *_d->tilemap; }

}//end namespace gfx
