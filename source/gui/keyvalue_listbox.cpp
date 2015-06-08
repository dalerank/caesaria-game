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

#include "keyvalue_listbox.hpp"
#include "core/utils.hpp"
#include "listboxitem.hpp"
#include "core/osystem.hpp"
#include "widget_factory.hpp"

namespace gui
{

REGISTER_CLASS_IN_WIDGETFACTORY(KeyValueListBox)

KeyValueListBox::KeyValueListBox(Widget* parent)
  : ListBox( parent, Rect( 0, 0, 1, 1) )
{

}

KeyValueListBox::KeyValueListBox(Widget* parent, const Rect& rectangle, int id)
  : ListBox( parent, rectangle, id )
{
}

ListBoxItem& KeyValueListBox::addItem(const std::string& keyValue, Font font, const int color)
{
  StringArray items = utils::split( keyValue, "][" );
  foreach( it, items )
  {
    *it = utils::trim( *it, "]" );
    *it = utils::trim( *it, "[" );
  }
  ListBoxItem& item = addItem( items.valueOrEmpty( 0 ), items.valueOrEmpty( 1 ), font, color );

  return item;
}

ListBoxItem& KeyValueListBox::addItem(const std::string& key, const std::string& value, Font font, const int color)
{
  ListBoxItem& item = ListBox::addItem( " ", font, color );

  item.setData( "key", Variant( key ) );
  item.setData( "value", Variant( value ) );
  return item;
}

void KeyValueListBox::_updateItemText(gfx::Engine& painter, ListBoxItem& item, const Rect& textRect, Font font, const Rect& frameRect )
{
  ListBox::_updateItemText( painter, item, textRect, font, frameRect );

  Font f = Font::create( FONT_1 );

  std::string str = item.data( "key" ).toString();
  Rect finalRect = f.getTextRect( str, Rect( 0, 0, frameRect.width() / 2, frameRect.height() ), align::center, align::center );

  item.draw( str, f, finalRect.lefttop() - Point( 10, 0)  );

  str = item.data( "value" ).toString();
  finalRect = f.getTextRect( str, Rect( frameRect.width()/2, 0, frameRect.width(), frameRect.height() ), align::upperLeft, align::center );
  item.draw( str, f, finalRect.lefttop() - Point( 10, 0)  );
}


}//end namespace gui
