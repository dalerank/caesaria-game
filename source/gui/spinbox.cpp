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
#include "core/color_list.hpp"
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
  struct
  {
    int minimum = 0;
    int maximum = 100;
    int current = 0;
    int step = 10;
  } value;

  struct
  {
    Point decBtn;
    Point incBtn;
  } position;

  std::string postfix;
  TexturedButton* btnDecrease;
  TexturedButton* btnIncrease;

  struct {
    Signal1<int> onChange;
    Signal2<SpinBox*,int> onChangeA;
  } signal;

  Impl() : btnDecrease( 0 ), btnIncrease( 0 ) {}
};

//! constructor
SpinBox::SpinBox( Widget* parent )
    : Label( parent, Rect( 0, 0, 1, 1) ), _d( new Impl )
{
  _initButtons();
}

void SpinBox::_initButtons()
{
  _d->btnDecrease = &add<TexturedButton>( Point( width() * 0.5, 1 ), Size( 24, 24), -1, 601 );
  _d->btnIncrease = &add<TexturedButton>( Point( width() * 0.5 + 25, 1), Size( 24, 24 ), -1, 605 );

  CONNECT_LOCAL( _d->btnDecrease, onClicked(), SpinBox::_decrease )
  CONNECT_LOCAL( _d->btnIncrease, onClicked(), SpinBox::_increase )
}

void SpinBox::_finalizeResize()
{
  Label::_finalizeResize();
  if( _d->btnDecrease )
  {
    _d->btnDecrease->setPosition( Point( width() * 0.5, 1 ) );
    if( _d->position.decBtn.x() > 0 )
      _d->btnDecrease->setPosition( _d->position.decBtn );
  }

  if( _d->btnIncrease )
  {
    _d->btnIncrease->setPosition( Point( width() * 0.5 + 25, 1 ) );
    if( _d->position.incBtn.x() > 0 )
      _d->btnIncrease->setPosition( _d->position.incBtn );
  }
  _update();
}

SpinBox::SpinBox(Widget* parent, const Rect& rectangle, const std::string& text, const std::string& postfix, int id)
 : Label( parent, rectangle, text, false, bgNone, id ),
	_d( new Impl )
{ 
  _d->postfix = postfix;
  _initButtons();
#ifdef _DEBUG
   setDebugName( TEXT(SpinBox) );
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
  _d->value.current = math::clamp( value, _d->value.minimum, _d->value.maximum );
  _update();
}

Signal1<int>& SpinBox::onChange(){ return _d->signal.onChange;}
Signal2<SpinBox*,int>& SpinBox::onChangeA(){ return _d->signal.onChangeA; }

void gui::SpinBox::_updateTexture(Engine& painter)
{
  Label::_updateTexture( painter );
  _update();
}

void SpinBox::_increase()
{
  setValue( _d->value.current + _d->value.step );
  emit _d->signal.onChange( _d->value.current );
  emit _d->signal.onChangeA( this, _d->value.current );
}

void SpinBox::_decrease()
{
  setValue( _d->value.current - _d->value.step );
  emit _d->signal.onChange( _d->value.current );
  emit _d->signal.onChangeA( this, _d->value.current );
}

void SpinBox::_update()
{
  if( font().isValid() && _textPicture().isValid() )
  {
    _textPicture().fill( ColorList::clear, Rect() );
    canvasDraw( text(),
                Rect( Point( 0, 0 ), _d->btnDecrease->leftbottom()  ) );

    canvasDraw( fmt::format( "{} {}", _d->value.current, _d->postfix ),
                Rect( _d->btnIncrease->right() + 5, 0, width(), height() ) );
  }
}

void SpinBox::setupUI(const VariantMap& ui)
{
  Label::setupUI( ui );

  _d->value.maximum = ui.get( "max", _d->value.maximum );
  _d->value.minimum = ui.get( "min", _d->value.minimum );
  _d->value.current = ui.get( "value", _d->value.current );
  _d->postfix = ui.get( "postfix", _d->postfix ).toString();
  _d->position.decBtn = ui.get( "decpos" );
  _d->position.incBtn = ui.get( "incpos" );
}

}//end namespace gui
