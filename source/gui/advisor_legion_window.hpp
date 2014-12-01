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

#ifndef __CAESARIA_ADVISOR_LEGION_WINDOW_H_INCLUDED__
#define __CAESARIA_ADVISOR_LEGION_WINDOW_H_INCLUDED__

#include "window.hpp"
#include "core/signals.hpp"
#include "objects/predefinitions.hpp"
#include "city/predefinitions.hpp"

namespace gui
{

namespace advisorwnd
{

class Legion : public Window
{
public:
  Legion( Widget* parent, int id, PlayerCityPtr city, FortList forts);

  virtual void draw( gfx::Engine& painter );

private:
  void _handleMove2Legion( FortPtr fort );
  void _handleRetreaLegion( FortPtr fort );
  void _handleServiceEmpire( FortPtr fort );
  void _showHelp();

  class Impl;
  ScopedPtr< Impl > _d;
};

}//end namespace advisorwnd

}//end namespace gui
#endif
