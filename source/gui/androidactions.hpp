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

#ifndef _CAESARIA_ANDROIDACTIONSBAR_H_INCLUDE_
#define _CAESARIA_ANDROIDACTIONSBAR_H_INCLUDE_

#include "window.hpp"
#include "core/signals.hpp"

namespace gui
{

class AndroidActionsBar : public Window
{
public:
  virtual void beforeDraw( gfx::Engine& painter);
  AndroidActionsBar( Widget* parent );

public signals:
  Signal0<>& onRequestTileHelp();
  Signal0<>& onEscapeClicked();
  Signal0<>& onEnterClicked();
  Signal0<>& onRequestMenu();

private:
  class Impl;
  ScopedPtr<Impl> _d;
};

}//end namesapce gui

#endif //_CAESARIA_ANDROIDACTIONSBAR_H_INCLUDE_
