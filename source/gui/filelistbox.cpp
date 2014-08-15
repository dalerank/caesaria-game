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

#include "filelistbox.hpp"
#include "vfs/filesystem.hpp"
#include "core/stringhelper.hpp"
#include "listboxitem.hpp"

namespace gui
{

FileListBox::FileListBox(Widget* parent)
  : ListBox( parent, Rect( 0, 0, 1, 1) )
{

}

FileListBox::FileListBox(Widget* parent, const Rect& rectangle, int id)
  : ListBox( parent, rectangle, id )
{

}

ListBoxItem& FileListBox::addItem(const std::string& text, Font font, const int color)
{
  DateTime time = vfs::FileSystem::instance().getFileUpdateTime( text );
  std::string timeStr = StringHelper::format( 0xff, "(%02d %s %02d:%02d:%02d)",
                                              time.day(), DateTime::getShortMonthName( time.month()-1 ),
                                              time.hour(), time.minutes(), time.seconds() );
  ListBoxItem& item = ListBox::addItem( vfs::Path( text ).baseName().toString(), font, color );

  item.setData( Variant( timeStr ) );
  return item;
}


void FileListBox::_updateItemText(gfx::Engine& painter, ListBoxItem& item, const Rect& textRect, Font font, const Rect& frameRect )
{
  ListBox::_updateItemText( painter, item, textRect, font, frameRect );

  Font f = Font::create( FONT_1 );

  std::string timeStr = item.data().toString();
  Rect finalRect = f.getTextRect( timeStr, Rect( Point(), frameRect.size() ), align::lowerRight, align::center );

  item.draw( timeStr, f, finalRect.lefttop() - Point( 10, 0)  );
}


}//end namespace gui
