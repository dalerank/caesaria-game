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

#include "layersimple.hpp"
#include "layerconstants.hpp"
#include "walker/constants.hpp"
#include "city_renderer.hpp"
#include "city/city.hpp"
#include "gui/senate_popup_info.hpp"
#include "objects/senate.hpp"

using namespace constants;

namespace gfx
{

class LayerSimple::Impl
{
public:
  SenatePopupInfo senateInfo;
};

int LayerSimple::type() const { return citylayer::simple; }

LayerPtr LayerSimple::create( Camera& camera, PlayerCityPtr city)
{
  LayerPtr ret( new LayerSimple( camera, city ) );
  ret->drop();

  return ret;
}

void LayerSimple::renderUi(Engine &engine)
{
  Layer::renderUi( engine );

  Tile* lastTile = _currentTile();
  if( lastTile )
  {
    TileOverlayPtr ov = lastTile->overlay();
    SenatePtr senate = ptr_cast<Senate>( ov );
    if( senate.isValid() )
    {
      _d->senateInfo.draw( _lastCursorPos(), Engine::instance(), senate );
    }
  }
}

LayerSimple::LayerSimple( Camera& camera, PlayerCityPtr city)
  : Layer( &camera, city ), _d( new Impl )
{
  _addWalkerType( walker::all );
}

}//end namespace gfx
