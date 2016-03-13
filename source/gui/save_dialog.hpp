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

#ifndef __CAESARIA_SAVE_DIALOG_H_INCLUDED__
#define __CAESARIA_SAVE_DIALOG_H_INCLUDED__

#include "window.hpp"
#include "core/scopedptr.hpp"
#include "core/signals.hpp"
#include "vfs/directory.hpp"

namespace gui
{

class ListBoxItem;

namespace dialog
{

class SaveGame : public Window
{
public:
  SaveGame(Widget* parent);
  SaveGame(Ui* ui, vfs::Directory dir, std::string fileExt, int id);

  void setDirectory(const std::string& dir);
  void setFilter(const std::string& filter);

  virtual void draw(gfx::Engine& painter);

signals public:
  Signal1<std::string>& onFileSelected();
  Signal2<Widget*,std::string>& onFileSelectedEx();

private:
  void _resolveListboxChange(const ListBoxItem &item);
  void _resolveOkClick();
  void _resolveDblListboxChange(const ListBoxItem &item);
  void _save();

  class Impl;
  ScopedPtr< Impl > _d;
};

}//end namespace dialog

}//end namespace gui
#endif //__CAESARIA_SAVE_DIALOG_H_INCLUDED__
