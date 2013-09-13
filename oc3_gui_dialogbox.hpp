// This file is part of openCaesar3.
//
// openCaesar3 is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// openCaesar3 is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with openCaesar3.  If not, see <http://www.gnu.org/licenses/>.

#ifndef _OPENCAESAR3_GUI_DIALOGBOX_INCLUDE_H_
#define _OPENCAESAR3_GUI_DIALOGBOX_INCLUDE_H_

#include "oc3_gui_widget.hpp"
#include "oc3_signals.hpp"
#include "oc3_scopedptr.hpp"

class DialogBox : public Widget 
{
public:
  enum { btnYes=1, btnNo=2, btnOk=4, btnCancel=8 };
  DialogBox( Widget* parent, const Rect& rectangle, const std::string& title, 
             const std::string& text, int buttons );

  bool onEvent(const NEvent& event);

  void draw( GfxEngine& painter );

oc3_signals public:  
  Signal1<int>& onResult();
  Signal0<>& onOk();
  Signal0<>& onCancel();

private:
  class Impl;
  ScopedPtr< Impl > _d;
};

#endif
