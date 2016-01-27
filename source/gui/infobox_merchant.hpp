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
// Copyright 2012-2015 Dalerank, dalerankn8@gmail.com

#ifndef _CAESARIA_INFOBOX_MERCHANT_H_INCLUDE_
#define _CAESARIA_INFOBOX_MERCHANT_H_INCLUDE_

#include "infobox_citizen.hpp"

namespace gui
{

namespace infobox
{

namespace citizen
{

// Simple info box with static text on plain background
class AboutMerchant : public AboutPeople
{
public:
  AboutMerchant(Widget* parent, PlayerCityPtr city, const TilePos& pos);

protected:
  void _drawGood(const good::Product &goodType, int qty, int index, int paintY);
  virtual void _updateExtInfo();
};

}//citizen

}//infobox

}//end namespace gui
#endif //_CAESARIA_INFOBOX_MERCHANT_H_INCLUDE_
