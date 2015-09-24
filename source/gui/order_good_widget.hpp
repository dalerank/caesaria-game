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

#ifndef __CAESARIA_GOOD_ORDERS_WIDGET_H_INCLUDED__
#define __CAESARIA_GOOD_ORDERS_WIDGET_H_INCLUDED__

#include "label.hpp"
#include "good/store.hpp"

namespace gui
{

class PushButton;
class VolumeButton;

class OrderGoodWidget : public Label
{
public:
  static OrderGoodWidget* create( const int index, const good::Product good, Widget* parent, good::Store& storage );

  void changeCapacity( float fillingPercentage );
  void updateBtnText();
  void changeRule();
  void draw(gfx::Engine& painter);

protected:
  OrderGoodWidget( Widget* parent, const Rect& rect, good::Product good, good::Store& storage );
  virtual void _updateTexture( gfx::Engine& painter );

private:
  good::Product _type;
  good::Store& _storage;
  PushButton* _btnChangeRule;
  VolumeButton* _btnVolume;
};

}//end namesapce gui
#endif //__CAESARIA_GOOD_ORDERS_WIDGET_H_INCLUDED__
