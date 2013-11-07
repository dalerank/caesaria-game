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

#ifndef __OPENCAESAR3_LAYERHEALTH_H_INCLUDED__
#define __OPENCAESAR3_LAYERHEALTH_H_INCLUDED__

#include "layer.hpp"
#include "city_renderer.hpp"

class LayerHealth : public Layer
{
public:
  virtual int getType() const;
  virtual VisibleWalkers getVisibleWalkers() const;
  virtual void drawTile( GfxEngine& engine, Tile& tile, Point offset );

  static LayerPtr create( CityRenderer* renderer, CityPtr city, int type );
private:
  CityRenderer* _renderer;
  CityPtr _city;
  std::set<int> _flags;
  std::set<int> _walkers;
  int _type;
};

#endif //__OPENCAESAR3_LAYERHEALTH_H_INCLUDED__
