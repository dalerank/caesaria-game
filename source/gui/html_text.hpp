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

#ifndef __CAESARIA_HTML_TEXT_H_INCLUDE_
#define __CAESARIA_HTML_TEXT_H_INCLUDE_

#include "image.hpp"
#include "gfx/picturesarray.hpp"
#include "core/signals.hpp"

namespace gui
{

class HtmlText : public Image
{
public:
  //! constructor
  HtmlText( Widget* parent );

  HtmlText( Widget* parent, const Rect& rectangle, const std::string& uri="");

  //! destructor
  virtual ~HtmlText();

  //! draws the element and its children
  virtual void draw( gfx::Engine& painter );
private:
  void _init();

  class Impl;
  ScopedPtr< Impl > _d;
};

}//end namespace gui

#endif//__CAESARIA_HTML_TEXT_H_INCLUDE_
