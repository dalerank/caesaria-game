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

#ifndef __CAESARIA_MINIMAP_WINDOW_H_INCLUDE_
#define __CAESARIA_MINIMAP_WINDOW_H_INCLUDE_

#include "widget.hpp"
#include "core/scopedptr.hpp"
#include "core/signals.hpp"
#include "city/predefinitions.hpp"

namespace gfx
{
  class Tilemap;
  class Camera;
}

namespace gui
{

class Minimap : public Widget
{
public:
  Minimap(Widget* parent);
  Minimap(Widget* parent, const Rect& rect, PlayerCityPtr city,
          const Size& size=Size() );

  void setCenter(Point pos);
  void setTileCenter(const TilePos& tpos);

  virtual void draw( gfx::Engine& painter);
  virtual bool onEvent(const NEvent &event);
  virtual void beforeDraw( gfx::Engine& painter );

  void setCity(PlayerCityPtr city);
  void setSize(const Size& size);
  void saveImage(const std::string& filename) const;
  void update();

public signals:
  Signal1<TilePos>& onCenterChange();
  Signal2<Widget*, TilePos>& onCenterChangeEx();
  Signal1<int>& onZoomChange();
  Signal2<Widget*, int>& onZoomChangeEx();

private:
  virtual bool _onMousePressed( const NEvent::Mouse& event);
  virtual void _finalizeResize();

  class Impl;
  ScopedPtr< Impl > _d;
};

}//end namespace gui
#endif //__CAESARIA_MINIMAP_WINDOW_H_INCLUDE_
