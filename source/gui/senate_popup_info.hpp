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

#ifndef __CAESARIA_SENATE_POPUP_INFO_H_INCLUDED__
#define __CAESARIA_SENATE_POPUP_INFO_H_INCLUDED__

#include "widget.hpp"
#include "core/scopedptr.hpp"

class CityRenderer;

namespace gui
{

class SenatePopupInfo : public Widget
{
public:
  SenatePopupInfo(Widget* parent, CityRenderer& mapRenderer );

  virtual void draw( GfxEngine& painter );
private:
  class Impl;
  ScopedPtr< Impl > _d;
};

}//end namespace gui

#endif //__CAESARIA_SENATE_POPUP_INFO_H_INCLUDED__
