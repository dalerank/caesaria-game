// this file was created by rt (www.tomkorp.com), based on ttk's png-reader
// i wanted to be able to read in PNG images with opencaesar :)

#ifndef __OC3_RENDERER_H_INCLUDED__
#define __OC3_RENDERER_H_INCLUDED__

#include "core/referencecounted.hpp"
#include "picture.hpp"
#include "core/smartptr.hpp"

//!  Surface Loader for PNG files
class Renderer : public ReferenceCounted
{
public:
  typedef enum {
                  none = 0, //non pass active
                  ground=12,                  
                  foreground = 24, //solid overlays
                  building=32,
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
};

class Renderable : public ReferenceCounted
{
public:
  virtual PicturesArray getPictures( Renderer::Pass ) const = 0;
};

#endif //__OC3_RENDERER_H_INCLUDED__

