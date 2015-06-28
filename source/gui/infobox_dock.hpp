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

#ifndef __CAESARIA_INFOBOX_DOCK_H_INCLUDE_
#define __CAESARIA_INFOBOX_DOCK_H_INCLUDE_

#include "infobox_construction.hpp"

namespace gui
{

namespace infobox
{

// info box about a market
class AboutDock : public AboutConstruction
{
public:
   AboutDock( Widget* parent, PlayerCityPtr city, const gfx::Tile& tile );
   virtual ~AboutDock();
   
   void drawGood(DockPtr dock, const good::Product& goodType, int, int );
   void updateStore(DockPtr dock);
};

}

}//end namespace gui
#endif //__CAESARIA_INFOBOX_DOCK_H_INCLUDE_
