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

#ifndef _OPENCAESAR3_WAREHOUSE_INFOBOX_H_INCLUDE_
#define _OPENCAESAR3_WAREHOUSE_INFOBOX_H_INCLUDE_

#include "info_box.hpp"

namespace gui
{

class InfoBoxWarehouse : public InfoboxSimple
{
public:
  InfoBoxWarehouse( Widget* parent, const Tile& tile );
  virtual ~InfoBoxWarehouse();

  void drawGood( const Good::Type &goodType, int col, int paintY);
  void showSpecialOrdersWindow();

private:
  WarehousePtr _warehouse;
};

}//end namespace gui
#endif //_OPENCAESAR3_WAREHOUSE_INFOBOX_H_INCLUDE_
