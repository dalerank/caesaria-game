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

#ifndef __CAESARIA_FILELISTBOX_H_INCLUDED__
#define __CAESARIA_FILELISTBOX_H_INCLUDED__

#include "listbox.hpp"
#include "core/scopedptr.hpp"
#include "vfs/path.hpp"
#include "core/signals.hpp"

namespace gui
{

class FileListBox : public ListBox
{
public:
  FileListBox( Widget* parent );
  FileListBox( Widget* parent, const Rect& rectangle, int id );

  void setShowTime( bool show );
  void setShowExtension( bool show );

  virtual ListBoxItem& addItem(const std::string &text, Font font, const int color);

protected:
  virtual void _updateItemText(gfx::Engine& painter, ListBoxItem& item, const Rect& textRect, Font font, const Rect& frameRect );
};

}//end namespace gui
#endif //__CAESARIA_FILELISTBOX_H_INCLUDED__
