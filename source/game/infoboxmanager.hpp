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
// Copyright 2012-2013 dalerank, dalerankn8@gmail.com

#ifndef __CAESARIA_INFOBOX_MANAGER_H_INCLUDE_
#define __CAESARIA_INFOBOX_MANAGER_H_INCLUDE_

#include "core/smartptr.hpp"
#include "core/referencecounted.hpp"
#include "core/scopedptr.hpp"
#include "core/predefinitions.hpp"
#include "enums.hpp"
#include "gui/info_box.hpp"

class InfoboxManager;
typedef SmartPtr< InfoboxManager > InfoBoxManagerPtr;

class InfoboxCreator
{
public:
  virtual gui::InfoboxSimple* create( PlayerCityPtr, gui::Widget*, TilePos ) = 0;
};

class InfoboxManager : public ReferenceCounted
{
public:
  static InfoboxManager& getInstance();

  void showHelp( PlayerCityPtr city, gui::GuiEnv* gui, TilePos tile );
  void setShowDebugInfo( const bool showInfo );

  void addInfobox( const gfx::TileOverlay::Type type, const std::string& typeName, InfoboxCreator* ctor );
  bool canCreate( const gfx::TileOverlay::Type type ) const;
private:
  InfoboxManager();
  ~InfoboxManager();
   
  class Impl;
  ScopedPtr< Impl > _d;
};

#endif //__CAESARIA_INFOBOX_MANAGER_H_INCLUDE_
