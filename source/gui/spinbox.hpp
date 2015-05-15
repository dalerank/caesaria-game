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

#ifndef __CAESARIA_GUISPINBOX_H_INCLUDE_
#define __CAESARIA_GUISPINBOX_H_INCLUDE_

#include "label.hpp"

namespace gui
{

class SpinBox : public Label
{
public:
  //! constructor
  SpinBox(Widget* parent );

  SpinBox(Widget* parent, const Rect& rectangle, const std::string& text="", const std::string& postfix="", int id=-1);

  //! destructor
  virtual ~SpinBox();

  //! draws the element and its children
  virtual void draw( gfx::Engine& painter );
  virtual void setValue( int value );
  virtual void setupUI( const VariantMap& ui );
    
signals public:
  virtual Signal1<int>& onChange();
  virtual Signal2<SpinBox*,int>& onChangeA();

private:
  void _updateTexture(gfx::Engine& painter);
  void _increase();
  void _decrease();
  void _update();
  void _initButtons();
  virtual void _finalizeResize();

  class Impl;
  ScopedPtr< Impl > _d;
};

}//end namespace gui

#endif //__CAESARIA_GUISPINBOX_H_INCLUDE_
