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

#ifndef _CAESARIA_GUI_DIALOGBOX_INCLUDE_H_
#define _CAESARIA_GUI_DIALOGBOX_INCLUDE_H_

#include "window.hpp"
#include "core/signals.hpp"
#include "core/scopedptr.hpp"

namespace  gui
{

class Dialogbox : public Window
{
public:
  enum { btnYes=0x1, btnNo=0x2, btnYesNo=btnYes|btnNo,
         btnNever=0x10 };

  Dialogbox( Widget* parent );
  Dialogbox( Ui* ui, const Rect& rectangle, const std::string& title,
             const std::string& text, int buttons);

  virtual bool onEvent(const NEvent& event);
  virtual void setupUI(const VariantMap &ui);
  virtual void draw(gfx::Engine& painter);
  virtual void setTitle(const std::string& title);
  virtual void setText(const std::string &text);
  void setButtons(int buttons);

signals public:  
  Signal1<int>& onResult();
  Signal0<>& onYes();
  Signal1<Widget*>& onYesEx();
  Signal0<>& onNo();
  Signal1<Widget*>& onNoEx();
  Signal1<bool>& onNever();
  Signal2<Widget*,bool>& onNeverEx();

private:
  void _initSimpleDialog();

  class Impl;
  ScopedPtr< Impl > _d;
};

namespace dialog
{

Dialogbox& Information(Ui* ui,
                      const std::string& title,
                      const std::string& text,
                      bool showNever=false);

Dialogbox& Confirmation( Ui* ui,
                      const std::string& title,
                      const std::string& text);

Dialogbox& Confirmation( Ui* ui,
                      const std::string& title,
                      const std::string& text ,
                      Callback callback);

Dialogbox& Confirmation( Ui* ui,
                      const std::string& title,
                      const std::string& text ,
                      Callback callbackOk,
                      Callback callbackCancel);


}//end namespace dialog

}//end namespace gui
#endif //_CAESARIA_GUI_DIALOGBOX_INCLUDE_H_
