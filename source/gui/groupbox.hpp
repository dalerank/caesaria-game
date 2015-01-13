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

#ifndef __CAESARIA_GROUPBOX_H_INCLUDED__
#define __CAESARIA_GROUPBOX_H_INCLUDED__

#include "widget.hpp"

namespace gui
{

class GroupBox : public Widget
{
public:
  typedef enum { whiteFrame=0, blackFrame, none, count } Style;

  //! constructor
  GroupBox( Widget* parent );
	GroupBox( Widget* parent, const Rect& rectangle, int id, Style style );

  //! destructor
  virtual ~GroupBox();

  //! sets an background image
  virtual void setBackgroundImage( const gfx::Picture& image);

  //! Gets the background image
  virtual const gfx::Picture& backgroundImage() const;

  //! sets if the image should scale to fit the element
  virtual void setScaleBackgroundImage(bool alignScale);
  virtual bool isBackgroundImageScaled() const;

  //! draws the element and its children
  virtual void draw( gfx::Engine& painter );

  virtual void setStyle( Style style );

  virtual void beforeDraw( gfx::Engine& painter );

  virtual void setupUI(const VariantMap &ui);
private:
	class Impl;
	ScopedPtr< Impl > _d;
};

}//end namespace gui
#endif //__CAESARIA_GROUPBOX_H_INCLUDED__
