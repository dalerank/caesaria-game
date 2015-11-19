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
#include "gfx/tilepos.hpp"

namespace scene { class Base; }

namespace gui
{

namespace tablet
{

class ActionsBar : public Window
{
public:
  virtual void beforeDraw( gfx::Engine& painter);
  virtual bool onEvent(const NEvent &event);
  ActionsBar( Widget* parent );

public signals:
  Signal0<>& onRequestTileHelp();
  Signal0<>& onEscapeClicked();
  Signal0<>& onEnterClicked();
  Signal0<>& onRequestMenu();
  Signal1<int>& onChangeZoom();

private:
  class Impl;
  ScopedPtr<Impl> _d;
};

class ActionsHandler : public Widget
{
public:
  static void assignTo( ActionsBar* parent, scene::Base* scene );

private slots:
  void _resolveEscapeButton();
  void _showTileHelp();
  void _resolveEnterButton();
  void _resolveExitGame();
  void _showSaveDialog();
  void _showLoadDialog();
  void _restartGame();
  void _exitToMainMenu();
  void _setActiveTile( TilePos tilepos );
  void _showIngameMenu();

private:
  void _sendKeyboardEvent(int key, bool ctrl=false);

  ActionsHandler( Widget* parent, scene::Base* scene );
  scene::Base* _scene;
  TilePos _tilepos;
};

}//end namespace tablet

}//end namesapce gui

#endif //_CAESARIA_ANDROIDACTIONSBAR_H_INCLUDE_
