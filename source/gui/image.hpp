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


#ifndef __CAESARIA_GUIIMAGE_H_INCLUDE_
#define __CAESARIA_GUIIMAGE_H_INCLUDE_

#include "widget.hpp"
#include "gfx/picture.hpp"
#include "core/signals.hpp"

namespace gui
{

class Image : public Widget
{
public:
  typedef enum { native=0, fit, image } Mode;
  //! constructor
  Image(Widget* parent );

  Image(Widget* parent, Rect rectangle, Picture pic, Mode mode=image, int id=-1);
  Image(Widget* parent, Point pos, Picture pic, int id=-1);

  //! destructor
  virtual ~Image();

  //! draws the element and its children
  virtual void draw( GfxEngine& painter );

  virtual void beforeDraw( GfxEngine& painter );

  virtual void setPicture( Picture picture );

  virtual void setupUI( const VariantMap& ui );
    
oc3_signals public:
  virtual Signal0<>& onClicked();

protected:
  virtual void _resizeEvent();
  virtual void _updateTexture( GfxEngine& );
  PictureRef& getPicture();

private:

  class Impl;
  ScopedPtr< Impl > _d;
};

}//end namespace gui

#endif //__CAESARIA_GUIIMAGE_H_INCLUDE_
