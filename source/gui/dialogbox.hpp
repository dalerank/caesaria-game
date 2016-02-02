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

namespace dialog
{

class Dialog : public Window
{
public:
  enum { stillPlay=0, pauseGame=1 };
  enum { btnYes=0x1, btnNo=0x2, btnOk=0x4, btnCancel=0x8,
         btnOkCancel=btnOk|btnCancel,
         btnNever=0x10 };

  Dialog( Ui* ui, const Rect& rectangle, const std::string& title,
             const std::string& text, int buttons, bool lockGame=false );

  virtual bool onEvent(const NEvent& event);
  virtual void setupUI(const VariantMap &ui);
  virtual void draw( gfx::Engine& painter );

signals public:  
  Signal1<int>& onResult();
  Signal0<>& onOk();
  Signal0<>& onCancel();
  Signal0<> &onNever();

private:
  class Impl;
  ScopedPtr< Impl > _d;
};

Dialog* Information(  Ui* ui,
                      const std::string& title,
                      const std::string& text );

Dialog* Confirmation( Ui* ui,
                      const std::string& title,
                      const std::string& text ,
                      bool pauseGame = false);

Dialog* Confirmation( Ui* ui,
                      const std::string& title,
                      const std::string& text ,
                      Callback callback,
                      bool pauseGame = false);

Dialog* Confirmation( Ui* ui,
                      const std::string& title,
                      const std::string& text ,
                      Callback callbackOk,
                      Callback callbackCancel,
                      bool pauseGame = false);


}//end namespace dialog

}//end namespace gui
#endif //_CAESARIA_GUI_DIALOGBOX_INCLUDE_H_
