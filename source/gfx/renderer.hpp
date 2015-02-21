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

#ifndef __CAESARIA_RENDERER_H_INCLUDED__
#define __CAESARIA_RENDERER_H_INCLUDED__

#include "core/referencecounted.hpp"
#include "core/smartptr.hpp"

#include <vector>

namespace gfx
{

class Camera;
namespace  layer {
  class Layer;
}

//!  Surface Loader for PNG files
class Renderer : public ReferenceCounted
{
public:
  typedef enum {
                  none = 0, //non pass active
                  ground=12,
                  groundAnimation,
                  overlay=32,//solid overlays
                  overlayAnimation,
                  overWalker,
                  animations=48,
                  transparent = 56, //transparent overlays
                  shadow = 64 //drawn after solid overlays                  
               } Pass;

  typedef std::vector<Pass> PassQueue;

  class Mode : public ReferenceCounted
  {
  public:
    virtual ~Mode() {}
  };

  typedef SmartPtr< Mode > ModePtr;
  virtual void render() = 0;
  virtual Camera* camera() = 0;
  virtual SmartPtr<layer::Layer> currentLayer() const = 0;
  virtual SmartPtr<layer::Layer> getLayer(int type) const = 0;
  virtual Renderer::ModePtr mode() const = 0;
};

}//end namespace gfx

#endif //__CAESARIA_RENDERER_H_INCLUDED__

