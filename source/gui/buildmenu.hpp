// This file is part of openCaesar3.
//
// openCaesar3 is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// openCaesar3 is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with openCaesar3.  If not, see <http://www.gnu.org/licenses/>.

#ifndef __OPENCAESAR_BUILDMENU_H_INCLUDE_
#define __OPENCAESAR_BUILDMENU_H_INCLUDE_

#include "gui/widget.hpp"
#include "game/enums.hpp"
#include "game/build_options.hpp"

namespace gui
{

class PushButton;

class BuildMenu : public Widget
{
public:
  static const int subMenuCreateIdHigh = 0x1000;

  BuildMenu( Widget* parent, const Rect& rectangle, int id );
  virtual ~BuildMenu();

  static BuildMenu* create( const BuildMenuType menuType, 
                            Widget* parent );

  // add the subMenu in the menu.
  void addSubmenuButton(const BuildMenuType menuType, const std::string &text);

  // add the button in the menu.
  void addBuildButton(const TileOverlayType buildingType);

  bool isPointInside(const Point& point) const;

  virtual void initialize();

  void setBuildOptions(const CityBuildOptions& options );

protected:
  CityBuildOptions _options;
};

class BuildMenu_water : public BuildMenu
{
public:
  BuildMenu_water( Widget* parent, const Rect& rectangle );
  void initialize();
};


class BuildMenu_security : public BuildMenu
{
public:
  BuildMenu_security( Widget* parent, const Rect& rectangle );
  void initialize();
};


class BuildMenu_education : public BuildMenu
{
public:
    BuildMenu_education( Widget* parent, const Rect& rectangle );
    void initialize();
};


class BuildMenu_health : public BuildMenu
{
public:
    BuildMenu_health( Widget* parent, const Rect& rectangle );
    void initialize();
};


class BuildMenu_engineering : public BuildMenu
{
public:
    BuildMenu_engineering( Widget* parent, const Rect& rectangle );
    void initialize();
};


class BuildMenu_administration : public BuildMenu
{
public:
    BuildMenu_administration( Widget* parent, const Rect& rectangle );
    void initialize();
};


class BuildMenu_entertainment : public BuildMenu
{
public:
    BuildMenu_entertainment( Widget* parent, const Rect& rectangle );
    void initialize();
};


class BuildMenu_commerce : public BuildMenu
{
public:
    BuildMenu_commerce( Widget* parent, const Rect& rectangle );
    void initialize();
};


class BuildMenu_farm : public BuildMenu
{
public:
    BuildMenu_farm( Widget* parent, const Rect& rectangle );
    void initialize();
};


class BuildMenu_raw_factory : public BuildMenu
{
public:
    BuildMenu_raw_factory( Widget* parent, const Rect& rectangle );
    void initialize();
};


class BuildMenu_factory : public BuildMenu
{
public:
    BuildMenu_factory( Widget* parent, const Rect& rectangle );
    void initialize();
};

class BuildMenu_religion: public BuildMenu
{
public:
    BuildMenu_religion( Widget* parent, const Rect& rectangle );
    void initialize();
};

class BuildMenu_temple : public BuildMenu
{
public:
    BuildMenu_temple( Widget* parent, const Rect& rectangle );
    void initialize();
};

class BuildMenu_bigtemple : public BuildMenu
{
public:
    BuildMenu_bigtemple( Widget* parent, const Rect& rectangle );
    void initialize();
};

}//end namespace gui
#endif //__OPENCAESAR_BUILDMENU_H_INCLUDE_
