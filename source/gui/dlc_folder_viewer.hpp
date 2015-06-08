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

#ifndef __CAESARIA_DLCFOLDERVIEWER_LISTBOX_H_INCLUDE__
#define __CAESARIA_DLCFOLDERVIEWER_LISTBOX_H_INCLUDE__

#include "window.hpp"
#include "core/scopedptr.hpp"
#include "vfs/directory.hpp"

namespace gui
{

class DlcFolderViewer : public Window
{
public:
  DlcFolderViewer( Widget* parent );
  DlcFolderViewer( Widget* parent, vfs::Directory folder );

  virtual ~DlcFolderViewer();
  virtual void draw(gfx::Engine &painter);
  virtual void setupUI(const VariantMap &ui);

private:
  void _resolveCellClick( int row, int column );
  void _openFolder();
  void _loadDesc( vfs::Path path );

  class Impl;
  ScopedPtr<Impl> _d;
};

}//end namespace gui

#endif //__CAESARIA_DLCFOLDERVIEWER_LISTBOX_H_INCLUDE__
