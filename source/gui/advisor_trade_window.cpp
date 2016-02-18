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

#include "advisor_trade_window.hpp"
#include <GameEvents>
#include <GameGfx>
#include <GameGood>
#include <GameCore>
#include <GameGui>
#include <GameLogger>
#include "game/resourcegroup.hpp"
#include "objects/factory.hpp"
#include "city/trade_options.hpp"
#include "objects/warehouse.hpp"
#include "good/store.hpp"
#include "image.hpp"
#include "objects/constants.hpp"
#include "goodordermanage.hpp"
#include "events/showempiremapwindow.hpp"
#include "city/statistic.hpp"
#include "advisor_trade_infobutton.hpp"

using namespace gfx;
using namespace city;

namespace gui
{

namespace advisorwnd
{

class Trade::Impl
{
public:
  GroupBox* gbInfo;
  PlayerCityPtr city;
  good::ProductMap allgoods;

  bool getWorkState( good::Product gtype );
  void updateGoodsInfo();
  void showGoodOrderManageWindow( good::Product type );
};

void Trade::Impl::updateGoodsInfo()
{
  if( !gbInfo )
    return;

  for( auto& child : gbInfo->children() )
    child->deleteLater();

  Point startDraw( 0, 5 );
  Size btnSize( gbInfo->width(), 20 );
  trade::Options& copt = city->tradeOptions();
  int indexOffset=0;
  for( auto& gtype : good::all() )
  {
    trade::Order tradeState = copt.getOrder( gtype );
    if( tradeState == trade::disabled || gtype == good::none)
    {
      continue;
    }

    bool workState = getWorkState( gtype );
    int exportQty = copt.tradeLimit( trade::exporting, gtype ).ivalue();
    int importQty = copt.tradeLimit( trade::importing, gtype ).ivalue();

    if( city->tradeOptions().isStacking( gtype ) )
    {
      exportQty = importQty = 0;
      tradeState = trade::stacking;
    }
    
    auto& btn = gbInfo->add<TradeGoodInfo>( Rect( startDraw + Point( 0, btnSize.height()) * indexOffset, btnSize ),
                                            gtype, allgoods[ gtype ], workState, tradeState, exportQty, importQty );
    indexOffset++;
    btn.onClickedA().connect( this, &Impl::showGoodOrderManageWindow );
  }
}

void Trade::_showEmpireMap() { events::dispatch<events::ShowEmpireMap>( true ); }

bool Trade::Impl::getWorkState(good::Product gtype )
{
  bool industryActive = false;
  FactoryList producers = city->statistic().objects.producers<Factory>( gtype );

  for( auto factory : producers )
    industryActive |= factory->isActive();

  return producers.empty() ? true : industryActive;
}

void Trade::Impl::showGoodOrderManageWindow(good::Product type)
{
  int gmode = GoodOrderManageWindow::gmUnknown;
  gmode |= (city->statistic().goods.canImport( type ) ? GoodOrderManageWindow::gmImport : 0);
  gmode |= (city->statistic().goods.canProduce( type ) ? GoodOrderManageWindow::gmProduce : 0);

  Widget* p = gbInfo->parent();
  auto& wnd = p->add<GoodOrderManageWindow>( Rect( 0, 0, p->width() - 80, p->height() - 100 ),
                                             city, type, allgoods[ type ], (GoodOrderManageWindow::GoodMode)gmode );
  wnd.onOrderChanged().connect( this, &Impl::updateGoodsInfo );
}

void Trade::_showGoodsPriceWindow()
{
  events::dispatch<events::ScriptFunc>( "OnShowEmpirePrices" );
}

Trade::Trade(Widget* parent, PlayerCityPtr city)
: Base( parent, city, advisor::trading ), _d( new Impl )
{
  setupUI( ":/gui/tradeadv.gui" );

  _d->city = city;
  _d->allgoods = city->statistic().goods.details( false );

  GET_DWIDGET_FROM_UI( _d, gbInfo )

  LINK_WIDGET_LOCAL_ACTION( PushButton*, btnEmpireMap, onClicked(), Trade::_showEmpireMap );
  LINK_WIDGET_LOCAL_ACTION( PushButton*, btnPrices,    onClicked(), Trade::_showGoodsPriceWindow );

  _d->updateGoodsInfo();

  add<HelpButton>( Point( 12, height() - 39), "trade_advisor" );
}

void Trade::draw(gfx::Engine& painter )
{
  if( !visible() )
    return;

  Window::draw( painter );
}

}//end namespace advisorwnd

}//end namespace gui
