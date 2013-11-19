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

#ifndef __CAESARIA_INFOBOX_MANAGER_H_INCLUDE_
#define __CAESARIA_INFOBOX_MANAGER_H_INCLUDE_

#include "core/smartptr.hpp"
#include "core/referencecounted.hpp"
#include "core/scopedptr.hpp"
#include "core/predefinitions.hpp"
#include "enums.hpp"
#include "gui/info_box.hpp"

class InfoBoxManager;
typedef SmartPtr< InfoBoxManager > InfoBoxManagerPtr;

class InfoboxCreator
{
public:
  virtual gui::InfoBoxSimple* create( gui::Widget*, const Tile& ) = 0;
};

class InfoBoxManager : public ReferenceCounted
{
public:
  static InfoBoxManagerPtr create( PlayerCityPtr city, gui::GuiEnv* gui );

  void showHelp( const Tile& tile ); 
  void setShowDebugInfo( const bool showInfo );

  void addInfobox( const TileOverlay::Type type, const std::string& typeName, InfoboxCreator* ctor );
  bool canCreate( const TileOverlay::Type type ) const;
private:
  InfoBoxManager( PlayerCityPtr city, gui::GuiEnv* gui );
  ~InfoBoxManager();
   
  class Impl;
  ScopedPtr< Impl > _d;
};

#endif //__CAESARIA_INFOBOX_MANAGER_H_INCLUDE_
