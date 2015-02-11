// this file was created by rt (www.tomkorp.com), based on ttk's png-reader
// i wanted to be able to read in PNG images with opencaesar :)

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

