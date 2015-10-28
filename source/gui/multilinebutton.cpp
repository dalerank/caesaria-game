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

#include "multilinebutton.hpp"
#include "widget_factory.hpp"
#include "stretch_layout.hpp"
#include "label.hpp"

namespace gui
{

REGISTER_CLASS_IN_WIDGETFACTORY(MultilineButton)

MultilineButton::MultilineButton(Widget *parent, const Rect& rectangle, const StringArray& strs, int id)
  : PushButton( parent, rectangle, "", id )
{
  _layout = &add<VLayout>( Rect( 0, 0, width(), height()), -1 );
  setText( _strings );
}

MultilineButton::MultilineButton(Widget *parent)
  : PushButton( parent, Rect(), "", -1 )
{
  _layout = &add<VLayout>( Rect( 0, 0, width(), height()), -1 );
}

Label* MultilineButton::line( unsigned int index)
{
  Widgets children = _layout->children();
  if( index >= children.size() )
    return 0;

  Widgets::iterator it = children.begin();
  it += index;
  return safety_cast<Label*>( *it );
}

void MultilineButton::setLineFont(unsigned int index, Font font)
{
  Label* lb = line( index );
  if( lb )
    lb->setFont( font );
}

void gui::MultilineButton::setLineText(unsigned int index, const std::string &text)
{
  Label* lb = line( index );
  if( lb )
    lb->setText( text );
}

void MultilineButton::setText(const std::string& text)
{
  PushButton::setText( text );
}

void MultilineButton::setText(const StringArray& text)
{
  PushButton::setText( "" );
  _strings = text;
  Widgets children = _layout->children();
  for( auto&& child : children )
    child->remove();

  for( auto& it : _strings )
  {
    Label* lb = new Label( _layout, Rect(), it );
    lb->setTextAlignment( horizontalTextAlign(), verticalTextAlign() );
  }
}

void MultilineButton::_finalizeResize()
{
  PushButton::_finalizeResize();
  _layout->setGeometry( Rect( 0, 0, width(), height() ) );
}

}//end namespace gui
