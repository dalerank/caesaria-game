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

#ifndef __CAESARIA_LOADGAMEDIALOG_H_INCLUDED__
#define __CAESARIA_LOADGAMEDIALOG_H_INCLUDED__

#include "loadfiledialog.hpp"
#include "gfx/picturesarray.hpp"

namespace gui
{

class ListBoxItem;

namespace dialog
{

class LoadGame : public LoadFile
{
public:
  static LoadGame* create( Widget* parent, const vfs::Directory& dir );
  virtual ~LoadGame();

protected:
  LoadGame( Widget* parent, const vfs::Directory& dir );
  virtual void _fillFiles();
  void _showPreview( const ListBoxItem& item );

  gfx::Picture _previewImg;
};

}//end namespace dialog

}//end namespace gui
#endif //__CAESARIA_LOADGAMEDIALOG_H_INCLUDED__
