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
// Copyright 2012-2013 Dalerank, dalerankn8@gmail.com

#ifndef _CAESARIA_GUI_DIALOGBOX_INCLUDE_H_
#define _CAESARIA_GUI_DIALOGBOX_INCLUDE_H_

#include "window.hpp"
#include "core/signals.hpp"
#include "core/scopedptr.hpp"

namespace  gui
{

class DialogBox : public Window
{
public:
  enum { btnYes=0x1, btnNo=0x2, btnOk=0x4, btnCancel=0x8,
         btnOkCancel=btnOk|btnCancel,
         btnNever=0x10 };

  DialogBox( Widget* parent, const Rect& rectangle, const std::string& title, 
             const std::string& text, int buttons );

  bool onEvent(const NEvent& event);

  void draw( gfx::Engine& painter );

  static DialogBox* information( Widget* parent,
                                 const std::string& title,
                                 const std::string& text );
  static DialogBox* confirmation( Widget* parent,
                                  const std::string& title,
                                  const std::string& text );

signals public:  
  Signal1<int>& onResult();
  Signal0<>& onOk();
  Signal0<>& onCancel();
  Signal0<> &onNever();

private:
  class Impl;
  ScopedPtr< Impl > _d;
};

}//end namespace gui
#endif //_CAESARIA_GUI_DIALOGBOX_INCLUDE_H_
