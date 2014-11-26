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
// Copyright 2012-2014 dalerank, dalerankn8@gmail.com

#ifndef __CAESARIA_TOPMENU_H_INCLUDE_
#define __CAESARIA_TOPMENU_H_INCLUDE_

#include "gui/mainmenu.hpp"
#include "core/scopedptr.hpp"
#include "game/advisor.hpp"

class DateTime;

namespace gui
{

class TopMenu : public MainMenu
{
public:
  TopMenu( Widget* parent, const int height );

  // draw on screen
  void draw( gfx::Engine& engine );

  void setFunds( int value );
  void setPopulation( int value );

signals public:
  Signal0<>& onExit();
  Signal0<>& onSave();
  Signal0<>& onEnd();
  Signal0<>& onLoad();
  Signal0<>& onRestart();
  Signal0<>& onShowVideoOptions();
  Signal0<>& onShowSoundOptions();
  Signal0<>& onShowGameSpeedOptions();
  Signal0<>& onShowCityOptions();
  Signal1<constants::advisor::Type>& onRequestAdvisor();

private:
  class Impl;
  ScopedPtr< Impl > _d;
};

}//end namespace gui
#endif //__CAESARIA_TOPMENU_H_INCLUDE_
