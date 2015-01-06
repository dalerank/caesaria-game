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

#ifndef __CAESARIA_BUILDMENU_H_INCLUDE_
#define __CAESARIA_BUILDMENU_H_INCLUDE_

#include "gui/widget.hpp"
#include "city/build_options.hpp"

namespace gui
{

class PushButton;

class BuildMenu : public Widget
{
public:
  static const int subMenuCreateIdHigh = 0x1000;

  BuildMenu( Widget* parent, const Rect& rectangle, int id,
             city::development::Branch branch );
  virtual ~BuildMenu();

  static BuildMenu* create( const city::development::Branch menuType,
                            Widget* parent );

  // add the subMenu in the menu.
  void addSubmenuButton(const city::development::Branch menuType, const std::string &text);

  // add the button in the menu.
  void addBuildButton(const gfx::TileOverlay::Type buildingType);

  virtual bool isPointInside(const Point& point) const;

  virtual void initialize();

  void setBuildOptions(const city::development::Options& options );

protected:
  void _resolveButtonClick();

  city::development::Options _options;
  city::development::Branch _branch;
};

class BuildMenu_water : public BuildMenu
{
public:
  BuildMenu_water( Widget* parent, const Rect& rectangle );
};


class BuildMenu_security : public BuildMenu
{
public:
  BuildMenu_security( Widget* parent, const Rect& rectangle );
};


class BuildMenu_education : public BuildMenu
{
public:
  BuildMenu_education( Widget* parent, const Rect& rectangle );
};

class BuildMenu_health : public BuildMenu
{
public:
  BuildMenu_health( Widget* parent, const Rect& rectangle );
};


class BuildMenu_engineering : public BuildMenu
{
public:
  BuildMenu_engineering( Widget* parent, const Rect& rectangle );
};


class BuildMenu_administration : public BuildMenu
{
public:
  BuildMenu_administration( Widget* parent, const Rect& rectangle );
};


class BuildMenu_entertainment : public BuildMenu
{
public:
  BuildMenu_entertainment( Widget* parent, const Rect& rectangle );
};

class BuildMenu_commerce : public BuildMenu
{
public:
  BuildMenu_commerce( Widget* parent, const Rect& rectangle );
};

class BuildMenu_farm : public BuildMenu
{
public:
  BuildMenu_farm( Widget* parent, const Rect& rectangle );
};


class BuildMenu_raw_material : public BuildMenu
{
public:
  BuildMenu_raw_material( Widget* parent, const Rect& rectangle );
};


class BuildMenu_factory : public BuildMenu
{
public:
  BuildMenu_factory( Widget* parent, const Rect& rectangle );
};

class BuildMenu_religion: public BuildMenu
{
public:
  BuildMenu_religion( Widget* parent, const Rect& rectangle );
};

class BuildMenu_temple : public BuildMenu
{
public:
  BuildMenu_temple( Widget* parent, const Rect& rectangle );
};

class BuildMenu_bigtemple : public BuildMenu
{
public:
  BuildMenu_bigtemple( Widget* parent, const Rect& rectangle );
};

}//end namespace gui
#endif //__CAESARIA_BUILDMENU_H_INCLUDE_
