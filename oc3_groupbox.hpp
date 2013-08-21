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

#ifndef __OPENCAESAR3_GROUPBOX_H_INCLUDED__
#define __OPENCAESAR3_GROUPBOX_H_INCLUDED__

#include "oc3_widget.hpp"

class GroupBox : public Widget
{
public:
  typedef enum { whiteFrame=0, blackFrame, whiteWindow } Style;

  //! constructor
	GroupBox( Widget* parent, const Rect& rectangle, int id, Style style );

  //! destructor
  virtual ~GroupBox();

  //! sets an background image
  virtual void setBackgroundImage( const Picture& image);

  //! Gets the background image
  virtual const Picture& getBackgroundImage() const;

  //! sets if the image should scale to fit the element
  virtual void setScaleBackgroundImage(bool scale);
  virtual bool isBackgroundImageScaled() const;

  //! draws the element and its children
  virtual void draw( GfxEngine& painter );

  virtual void setStyle( Style style );

  virtual void beforeDraw( GfxEngine& painter );

  //! Writes attributes of the element.
  //virtual void save(VariantArray* out) const;

  //! Reads attributes of the element
  //virtual void load(VariantArray* in);


private:
	class Impl;
	ScopedPtr< Impl > _d;
};

#endif //__OPENCAESAR3_GROUPBOX_H_INCLUDED__
