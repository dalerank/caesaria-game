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

#include <cstdio>

#include <GameCore>
#include <GameLogger>
#include "order_good_widget.hpp"
#include "pushbutton.hpp"
#include "gfx/engine.hpp"
#include "good/helper.hpp"
#include "good/store.hpp"

using namespace gfx;

namespace gui
{

class VolumeButton : public PushButton
{
public:
  VolumeButton( Widget* );

  //! constructor
  VolumeButton( Widget* parent,
                const Rect& rectangle,
                int roomCap, int warehouseCap )
          : PushButton( parent, rectangle )
  {
    float prc = roomCap / (float)warehouseCap;
    if( prc <= 0.25 ) { _step = 1; }
    else if( prc <= 0.50 ) { _step = 2; }
    else if( prc <= 0.75 ) { _step = 3; }
    else _step =0;

    _icon.load( "whblock", 1 );
    setBackgroundStyle( PushButton::blackBorderUp );
    setFont( "FONT_2_WHITE" );
    _updateText();
  }

  virtual void draw( gfx::Engine& painter )
  {
    if( !visible() )
      return;

    PushButton::draw( painter );

    //for( int i=0; i < step; i++ )
    //  painter.draw( icon, absoluteRect().lefttop() + Point( 6 + i * icon.width()/2, 0)/*, &absoluteClippingRectRef() */ );
  }

  Signal1<float>& onChange() { return _onChangeSignal; }

protected:

  void _updateText()
  {
    _step = _step % 4;
    setText( _step == 0 ? "Any" : fmt::format( "{}/4", _step ) );
  }

  //! when some mouse button clicked
  virtual void _btnClicked()
  {
    PushButton::_btnClicked();

    _step = (_step+1) % 4;
    _updateText();
    emit _onChangeSignal( _step / 4.f );
  }

  int _step;
  Picture _icon;
  Signal1<float> _onChangeSignal;
};

OrderGoodWidget::OrderGoodWidget(Widget* parent, int index, good::Product good, good::Store& storage)
  : Label( parent, Rect( Point( 0, 25 ) * index, Size( parent->width(), 25 ) ), "" ),
    _storage( storage )
{
  _info = good::Info( good );
  _goodIcon = _info.picture();
  setFont( "FONT_1_WHITE" );

  _btnChangeRule = &add<PushButton>( Rect( 140, 0, 140 + 240, height() ), "", -1, false, PushButton::blackBorderUp );
  _btnVolume = &add<VolumeButton>( Rect( _btnChangeRule->righttop(), Size( 40, height() ) ),
                                   _storage.capacity( good ), _storage.capacity() );

  _btnChangeRule->setFont( "FONT_1_WHITE" );
  updateBtnText();

  CONNECT_LOCAL( _btnChangeRule, onClicked(), OrderGoodWidget::changeRule );
  CONNECT_LOCAL( _btnVolume,     onChange(),  OrderGoodWidget::changeCapacity );
}

void OrderGoodWidget::_updateTexture(Engine& painter)
{
  Label::_updateTexture( painter );

  std::string goodName = _( "##" + _info.name() + "##" );

  canvasDraw( goodName, Point( 55, 0 ) );
}

void OrderGoodWidget::draw(Engine& painter)
{
  Label::draw( painter );

  painter.draw( _goodIcon, absoluteRect().lefttop() + Point( 15, 0 ), &absoluteClippingRectRef() );
  painter.draw( _goodIcon, absoluteRect().righttop() - Point( 35, 0 ), &absoluteClippingRectRef() );
}

void OrderGoodWidget::changeCapacity(float fillingPercentage)
{
  int storeCap = _storage.capacity();
  _storage.setCapacity( _info.type(), storeCap * fillingPercentage );
}

void OrderGoodWidget::updateBtnText()
{
  good::Orders::Order rule = _storage.getOrder( _info.type() );
  if( rule > good::Orders::none )
  {
    Logger::warning( "OrderGoodWidget: unknown rule {0}", (int)rule );
    return;
  }

  const char* ruleName[] = { "##accept##", "##reject##", "##deliver##", "##none##" };
  _btnChangeRule->setFont( rule == good::Orders::reject ? "FONT_1_RED" : "FONT_1_WHITE" );
  _btnChangeRule->setText( _(ruleName[ rule ]) );
}

void OrderGoodWidget::changeRule()
{
  good::Orders::Order rule = _storage.getOrder( _info.type() );
  _storage.setOrder( _info.type(), good::Orders::Order( (rule+1) % (good::Orders::none)) );
  updateBtnText();
}

}//end namespace gui
