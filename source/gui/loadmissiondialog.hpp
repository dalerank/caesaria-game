// This file is part of CaesarIA.
//
// openCaesar3 is free software: you can redistribute it and/or modify
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

#ifndef __CAESARIA_LOADMISSIONDIALOG_H_INCLUDE_
#define __CAESARIA_LOADMISSIONDIALOG_H_INCLUDE_

#include "widget.hpp"
#include "core/signals.hpp"
#include "core/scopedptr.hpp"

namespace gui
{

namespace dialog
{

class LoadMission : public Widget
{
public:
  static LoadMission* create(Widget* parent, const vfs::Directory& dir );
        
  virtual void draw( gfx::Engine& engine );

signals public:
  Signal1<std::string>& onSelectFile();

private:
  class Impl;
  ScopedPtr< Impl > _d;

  LoadMission( Widget* parent, const vfs::Directory& dir );
};

}//end namespace dialog

}//end namespace gui

#endif //__CAESARIA_LOADMISSIONDIALOG_H_INCLUDE_
