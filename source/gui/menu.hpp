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

#ifndef __CAESARIA_MENU_H_INCLUDE_
#define __CAESARIA_MENU_H_INCLUDE_

#include "gui/widget.hpp"
#include "core/signals.hpp"
#include "core/scopedptr.hpp"
#include "game/predefinitions.hpp"

namespace gui
{

class PushButton;

class Menu : public Widget
{
public:
  static Menu* create( Widget* parent, int id, PlayerCityPtr city );

  // draw on screen
  virtual void minimize();
  virtual void maximize();

  virtual void draw( gfx::Engine& engine );

  virtual bool onEvent(const NEvent& event);

  bool unselectAll();

signals public:
  Signal1<int>& onCreateConstruction();
  Signal0<>& onRemoveTool();
  Signal0<>& onHide();

protected:
  class Impl;
  ScopedPtr< Impl > _d;

  Menu( Widget* parent, int id, const Rect& rectangle );
  void _createBuildMenu( int type, Widget* parent );
  PushButton* _addButton( int startPic, bool pushBtn, int yMul,
                          int id, bool haveSubmenu, int midPic,
                          const std::string& tooltip="" );
};

class ExtentMenu : public Menu
{
public:
  static ExtentMenu* create( Widget* parent, int id, PlayerCityPtr city );

  virtual bool onEvent(const NEvent& event);

  virtual void draw( gfx::Engine& engine );

  void toggleOverlayMenuVisible();

  void setAlarmEnabled( bool enabled );

slots public:
  void changeOverlay( int ovType );

signals public:
  Signal1<int>& onSelectOverlayType();
  Signal0<>& onEmpireMapShow();
  Signal0<>& onAdvisorsWindowShow();
  Signal0<>& onSwitchAlarm();
  Signal0<>& onMessagesShow();
  Signal0<>& onRotateRight();
  Signal0<>& onRotateLeft();
  Signal0<>& onMissionTargetsWindowShow();

protected:
  ExtentMenu( Widget* parent, int id, const Rect& rectangle );
};

}//end namespace gui
#endif //__CAESARIA_MENU_H_INCLUDE_
