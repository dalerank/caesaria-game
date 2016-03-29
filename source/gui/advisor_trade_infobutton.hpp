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

#ifndef __CAESARIA_ADVISOR_TRADE_INFOBUTTON_H_INCLUDED__
#define __CAESARIA_ADVISOR_TRADE_INFOBUTTON_H_INCLUDED__

#include "pushbutton.hpp"
#include "good/good.hpp"
#include "gfx/picturesarray.hpp"
#include "city/trade_options.hpp"

namespace gui
{

class TradeGoodInfo : public PushButton
{
public:
  TradeGoodInfo( Widget* parent, const Rect& rect, good::Product good, int qty, bool enable,
                 city::trade::Order trade, int exportQty, int importQty );

  virtual void draw(gfx::Engine &painter);
  Signal1<good::Product>& onClickedA();

  virtual ~TradeGoodInfo();

protected:
  virtual void _btnClicked();
  virtual void _updateTexture();

private:
  class Impl;
  ScopedPtr<Impl> _d;
};

}//end namespace gui

#endif //__CAESARIA_ADVISOR_TRADE_INFOBUTTON_H_INCLUDED__
