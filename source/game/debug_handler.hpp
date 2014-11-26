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

#ifndef _CAESARIA_DEBUGHANDLER_INCLUDE_H_
#define _CAESARIA_DEBUGHANDLER_INCLUDE_H_

#include "core/scopedptr.hpp"
#include "gui/mainmenu.hpp"
#include "scene/level.hpp"
#include "game.hpp"

class DebugHandler
{
public:
  DebugHandler();
  void insertTo( Game* game, gui::MainMenu* menu );

  ~DebugHandler();

public signals:
  Signal2<scene::Level*,bool>& onFailedMission();
  Signal2<scene::Level*,bool>& onWinMission();

private:
  class Impl;
  ScopedPtr<Impl> _d;
};

#endif //_CAESARIA_DEBUGHANDLER_INCLUDE_H_
