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

#ifndef __CAESARIA_LOADFILEDIALOG_H_INCLUDED__
#define __CAESARIA_LOADFILEDIALOG_H_INCLUDED__

#include "window.hpp"
#include "core/scopedptr.hpp"
#include "vfs/path.hpp"
#include "core/signals.hpp"

namespace gui
{

class FileListBox;
class ListBoxItem;

namespace dialog
{

class LoadFile : public Window
{
public:
  LoadFile( Widget* parent, const Rect& rect,
                  const vfs::Directory& dir, const std::string& ext,
                  int id );

  virtual ~LoadFile();

  virtual void draw( gfx::Engine& engine );  // draw on screen

  virtual bool onEvent( const NEvent& event);

  virtual bool isPointInside(const Point& point) const;

  void setTitle( const std::string& title );
  void setText( const std::string& text );

  void setMayDelete( bool mayDelete );
  void setShowExtension( bool  showExtension );
  bool isMayDelete() const;

signals public:
  Signal1<std::string>& onSelectFile();

protected:

  virtual void _fillFiles();
  void _resolveItemSelected( const ListBoxItem& item );
  void _resolveItemDblClick( const ListBoxItem& item );

  gui::FileListBox* _fileslbx() const;
  const vfs::Directory& _directory() const;
  const std::string& _extensions() const;

  class Impl;
  ScopedPtr< Impl > _d;
};

}//end namespace dialog

}//end namespace gui
#endif //__CAESARIA_LOADFILEDIALOG_H_INCLUDED__
