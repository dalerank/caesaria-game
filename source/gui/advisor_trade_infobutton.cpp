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

#include "advisor_trade_infobutton.hpp"
#include "good/helper.hpp"
#include "gfx/decorator.hpp"
#include "gfx/engine.hpp"
#include "core/format.hpp"
#include "core/gettext.hpp"
#include "core/utils.hpp"

using namespace gfx;
using namespace city;

namespace gui
{

class TradeGoodInfo::Impl
{
public:
  bool enable;
  city::trade::Order tradeOrder;
  struct
  {
    int value;
    int importing;
    int exporting;
  } qty;

  good::Info info;
  Picture picture;
  gfx::Pictures border;

signals public:
  Signal1<good::Product> onClickedASignal;
};

TradeGoodInfo::TradeGoodInfo(Widget *parent, const Rect &rect, good::Product good, int qty, bool enable, trade::Order trade, int exportQty, int importQty)
  : PushButton( parent, rect, "", -1, false, PushButton::noBackground ), _d( new Impl )
{
  _d->info = good::Info( good );
  _d->picture = _d->info.picture();
  _d->qty.value = qty;
  _d->enable = enable;
  _d->tradeOrder = trade;
  _d->qty.exporting = exportQty;
  _d->qty.importing = importQty;
  Decorator::draw( _d->border, Rect( 50, 0, width() - 50, height() ), Decorator::brownBorder );

  setFont( FONT_2_WHITE );
}

void TradeGoodInfo::draw(Engine &painter)
{
  PushButton::draw( painter );

  painter.draw( _d->picture, absoluteRect().lefttop() + Point( 15, 0) );
  painter.draw( _d->picture, absoluteRect().righttop() - Point( 20 + _d->picture.width(), 0 ) );

  if( _state() == stHovered )
    painter.draw( _d->border, absoluteRect().lefttop(), &absoluteClippingRectRef() );
}

Signal1<good::Product>& TradeGoodInfo::onClickedA() { return _d->onClickedASignal; }
TradeGoodInfo::~TradeGoodInfo(){}

void TradeGoodInfo::_btnClicked()
{
  PushButton::_btnClicked();

  emit _d->onClickedASignal( _d->info.type() );
}

void TradeGoodInfo::_updateTexture()
{
  PushButton::_updateTexture();

  if( _textPicture().isValid() )
  {
    Font f = font( _state() );
    Picture& textPic = _textPicture();
    f.draw( textPic, _( _d->info.name() ), 55, 0, true, false );
    f.draw( textPic, utils::i2str(_d->qty.value / 100), 190, 0, true, false );
    f.draw( textPic, _d->enable ? "" : _("##disable##"), 260, 0, true, false );

    std::string ruleName[] = { "##import##", "", "##export##", "##stacking##" };
    std::string tradeStateText = ruleName[ _d->tradeOrder ];
    switch( _d->tradeOrder )
    {
    case trade::noTrade:
    case trade::stacking:
    case trade::importing:
      if( _d->qty.importing == 0 )
        tradeStateText = _( ruleName[ _d->tradeOrder ] );
      else
        tradeStateText = fmt::format( "{0} {1}", _( ruleName[ _d->tradeOrder ] ), _d->qty.importing );
    break;

    case trade::exporting:
      if( _d->qty.exporting == 0)
        tradeStateText = _( ruleName[ _d->tradeOrder ] );
      else
        tradeStateText = fmt::format( "{0} {1}", _( ruleName[ _d->tradeOrder ] ), _d->qty.exporting );
    break;

    default: break;
    }

    f.draw( textPic, tradeStateText, 340, 0, true, false );
    textPic.update();
  }
}

}//end namespace gui
