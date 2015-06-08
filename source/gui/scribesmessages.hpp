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

#ifndef __CAESARIA_SCRIBES_MESSAGES_WINDOW_H_INCLUDED__
#define __CAESARIA_SCRIBES_MESSAGES_WINDOW_H_INCLUDED__

#include "core/scopedptr.hpp"
#include "game/predefinitions.hpp"
#include "window.hpp"
#include "core/signals.hpp"

namespace gui
{

namespace dialog
{

class ScribesMessages : public Window
{
public:
  ScribesMessages(Widget* parent, PlayerCityPtr city);
  virtual ~ScribesMessages();

  virtual void draw( gfx::Engine& painter );

private:
  void _showMessage( int index );
  void _removeMessage( int index );
  void _fillMessages();
  void _showHelp();

  class Impl;
  ScopedPtr< Impl > _d;
};

}//end namespace dialog

}//end namespace gui
#endif //__CAESARIA_SCRIBES_MESSAGES_WINDOW_H_INCLUDED__
