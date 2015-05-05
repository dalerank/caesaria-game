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

#include "spinbox.hpp"
#include "gfx/engine.hpp"
#include "gfx/decorator.hpp"
#include "core/variant_map.hpp"
#include "gfx/pictureconverter.hpp"
#include "core/color.hpp"
#include "widget_factory.hpp"
#include "core/utils.hpp"
#include "texturedbutton.hpp"
#include "core/logger.hpp"

using namespace std;
using namespace gfx;

namespace gui
{

REGISTER_CLASS_IN_WIDGETFACTORY(SpinBox)

class SpinBox::Impl
{
public:
  int minv, maxv, value, step;
  std::string postfix;
  TexturedButton* btnDecrease;
  TexturedButton* btnIncrease;

  Impl() : minv( 0 ), maxv( 100 ), value( 0 ), step( 10 ), btnDecrease( 0 ), btnIncrease( 0 ) {}

public signals:
  Signal1<int> onChangeSignal;
  Signal2<SpinBox*,int> onChangeASignal;
};

//! constructor
SpinBox::SpinBox( Widget* parent )
    : Label( parent, Rect( 0, 0, 1, 1) ), _d( new Impl )
{
}

SpinBox::SpinBox(Widget* parent, const Rect& rectangle, const std::string& text, const std::string& postfix, int id)
 : Label( parent, rectangle, text, false, bgNone, id ),
	_d( new Impl )
{ 
  _d->postfix = postfix;
  _d->btnDecrease = new TexturedButton( this, Point( width() / 2, 2 ), Size( 24, 24), -1, 601 );
  _d->btnIncrease = new TexturedButton( this, Point( width() / 2 + 28, 2), Size( 24, 24 ), -1, 605 );

  CONNECT( _d->btnDecrease, onClicked(), this, SpinBox::_decrease );
  CONNECT( _d->btnIncrease, onClicked(), this, SpinBox::_increase );
#ifdef _DEBUG
    setDebugName( CAESARIA_STR_A(SpinBox) );
#endif
}

//! destructor
SpinBox::~SpinBox() {}

//! draws the element and its children
void SpinBox::draw(gfx::Engine& painter )
{
  if ( !visible() )
    return;

  Label::draw( painter );
}

void SpinBox::setValue(int value)
{
  _d->value = math::clamp( _d->value - _d->step, _d->minv, _d->maxv );
  _update();
}

Signal1<int>& SpinBox::onChange(){ return _d->onChangeSignal;}
Signal2<SpinBox*,int>& SpinBox::onChangeA(){ return _d->onChangeASignal; }

void gui::SpinBox::_updateTexture(Engine& painter)
{
  Label::_updateTexture( painter );
  _update();
}

void SpinBox::_increase()
{
  setValue( _d->value + _d->step );
  emit _d->onChangeSignal( _d->value );
  emit _d->onChangeASignal( this, _d->value );
}

void SpinBox::_decrease()
{
  setValue( _d->value - _d->step );
  emit _d->onChangeSignal( _d->value );
  emit _d->onChangeASignal( this, _d->value );
}

void SpinBox::_update()
{
  Font f = font();
  if( f.isValid() )
  {
    Rect frameRect( Point( _d->btnIncrease->right() + 5, 0 ), rightbottom() );
    string valueText = utils::format( 0xff, "%d %s", _d->value, _d->postfix.c_str() );
    _textPictureRef()->fill( DefaultColors::clear, frameRect );

    if( !valueText.empty() )
    {
      Rect textRect = f.getTextRect( valueText, frameRect, horizontalTextAlign(), verticalTextAlign() );
      f.draw( *_textPictureRef(), valueText, textRect.lefttop(), true, true );
    }
  }
}

void SpinBox::setupUI(const VariantMap& ui)
{
  Label::setupUI( ui );

  _d->maxv = ui.get( "max", _d->maxv );
  _d->minv = ui.get( "min", _d->minv );
  _d->value = ui.get( "value", _d->value );
  _d->postfix = ui.get( "postfix", _d->postfix ).toString();

  if( _d->btnDecrease )
  {
    Point decBtnPos = ui.get( "decpos", _d->btnDecrease->lefttop() );
    _d->btnDecrease->move( decBtnPos );
  }

  if( _d->btnIncrease )
  {
    Point incBtnPos = ui.get( "incpos", _d->btnIncrease->lefttop() );
    _d->btnIncrease->move( incBtnPos );
  }
}

}//end namespace gui
