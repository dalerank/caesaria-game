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

#include "order_good_widget.hpp"
#include "pushbutton.hpp"
#include "core/logger.hpp"
#include "gfx/engine.hpp"
#include "good/helper.hpp"
#include "core/gettext.hpp"
#include "core/utils.hpp"

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
    setFont( Font::create( FONT_2_WHITE ) );
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
    setText( _step == 0 ? "Any" : utils::format( 0xff, "%d/4", _step ) );
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

gui::OrderGoodWidget::OrderGoodWidget(Widget* parent, const Rect& rect, good::Product good, good::Store& storage)
  : Label( parent, rect, "" ), _storage( storage )
{
  _type = good;
  setFont( Font::create( FONT_1_WHITE ) );

  _btnChangeRule = &add<PushButton>( Rect( 140, 0, 140 + 240, height() ), "", -1, false, PushButton::blackBorderUp );
  _btnVolume = &add<VolumeButton>( Rect( _btnChangeRule->righttop(), Size( 40, height() ) ),
                                   _storage.capacity( good ), _storage.capacity() );

  _btnChangeRule->setFont( Font::create( FONT_1_WHITE ) );
  updateBtnText();

  CONNECT( _btnChangeRule, onClicked(), this, OrderGoodWidget::changeRule );
  CONNECT( _btnVolume, onChange(), this, OrderGoodWidget::changeCapacity );
}

void OrderGoodWidget::_updateTexture(Engine& painter)
{
  Label::_updateTexture( painter );

  std::string goodName = _( "##" + good::Helper::getTypeName( _type ) + "##" );

  if( _textPicture().isValid() )
  {
    Font rfont = font();
    rfont.draw( _textPicture(), goodName, 55, 0 );
  }
}

void OrderGoodWidget::draw(Engine& painter)
{
  Label::draw( painter );

  Picture goodIcon = good::Helper::picture( _type );
  painter.draw( goodIcon, absoluteRect().lefttop() + Point( 15, 0 ), &absoluteClippingRectRef() );
  painter.draw( goodIcon, absoluteRect().righttop() - Point( 35, 0 ), &absoluteClippingRectRef() );
}

OrderGoodWidget*OrderGoodWidget::create(const int index, const good::Product good, Widget* parent, good::Store& storage)
{
  Point offset( 0, 25 );
  Size wdgSize( parent->width(), 25 );
  return new OrderGoodWidget( parent, Rect( offset * index, wdgSize), good, storage );
}

void gui::OrderGoodWidget::changeCapacity(float fillingPercentage)
{
  int storeCap = _storage.capacity();
  _storage.setCapacity( _type, storeCap * fillingPercentage );
}

void gui::OrderGoodWidget::updateBtnText()
{
  good::Orders::Order rule = _storage.getOrder( _type );
  if( rule > good::Orders::none )
  {
    Logger::warning( "OrderGoodWidget: unknown rule {0}", (int)rule );
    return;
  }

  const char* ruleName[] = { "##accept##", "##reject##", "##deliver##", "##none##" };
  _btnChangeRule->setFont( Font::create( rule == good::Orders::reject ? FONT_1_RED : FONT_1_WHITE ) );
  _btnChangeRule->setText( _(ruleName[ rule ]) );
}

void gui::OrderGoodWidget::changeRule()
{
  good::Orders::Order rule = _storage.getOrder( _type );
  _storage.setOrder( _type, good::Orders::Order( (rule+1) % (good::Orders::none)) );
  updateBtnText();
}

}//end namespace gui
