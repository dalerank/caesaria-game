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
#include "gfx/picture.hpp"
#include "gfx/decorator.hpp"
#include "core/gettext.hpp"
#include "good/helper.hpp"
#include "pushbutton.hpp"
#include "label.hpp"
#include "game/resourcegroup.hpp"
#include "core/utils.hpp"
#include "gfx/engine.hpp"
#include "core/gettext.hpp"
#include "groupbox.hpp"
#include "objects/factory.hpp"
#include "city/trade_options.hpp"
#include "objects/warehouse.hpp"
#include "good/store.hpp"
#include "texturedbutton.hpp"
#include "core/event.hpp"
#include "core/foreach.hpp"
#include "core/logger.hpp"
#include "image.hpp"
#include "objects/constants.hpp"
#include "empireprices.hpp"
#include "goodordermanage.hpp"
#include "widget_helper.hpp"
#include "city/statistic.hpp"
#include "dictionary.hpp"
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
  PushButton* btnEmpireMap;
  PushButton* btnPrices; 
  GroupBox* gbInfo;
  PlayerCityPtr city;
  good::ProductMap allgoods;

  bool getWorkState( good::Product gtype );
  void updateGoodsInfo();
  void showGoodOrderManageWindow( good::Product type );
  void showGoodsPriceWindow();
};

void Trade::Impl::updateGoodsInfo()
{
  if( !gbInfo )
    return;

  Widget::Widgets children = gbInfo->children();

  for( auto&& child : children ) { child->deleteLater(); }

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
    
    TradeGoodInfo* btn = new TradeGoodInfo( gbInfo, Rect( startDraw + Point( 0, btnSize.height()) * indexOffset, btnSize ),
                                            gtype, allgoods[ gtype ], workState, tradeState, exportQty, importQty );
    indexOffset++;
    CONNECT( btn, onClickedA(), this, Impl::showGoodOrderManageWindow );
  } 
}

bool Trade::Impl::getWorkState(good::Product gtype )
{
  bool industryActive = false;
  FactoryList producers = city->statistic().objects.producers<Factory>( gtype );

  for( auto factory : producers ) { industryActive |= factory->isActive(); }

  return producers.empty() ? true : industryActive;
}

void Trade::Impl::showGoodOrderManageWindow(good::Product type )
{
  int gmode = GoodOrderManageWindow::gmUnknown;
  Widget* p = gbInfo->parent();
  gmode |= (city->statistic().goods.canImport( type ) ? GoodOrderManageWindow::gmImport : 0);
  gmode |= (city->statistic().goods.canProduce( type ) ? GoodOrderManageWindow::gmProduce : 0);

  GoodOrderManageWindow* wnd = new GoodOrderManageWindow( p, Rect( 0, 0, p->width() - 80, p->height() - 100 ),
                                                          city, type, allgoods[ type ], (GoodOrderManageWindow::GoodMode)gmode );
  wnd->setCenter( p->center() );
  CONNECT( wnd, onOrderChanged(), this, Impl::updateGoodsInfo );
}

void Trade::Impl::showGoodsPriceWindow()
{
  Widget* parent = gbInfo->parent();
  Size size( 610, 180 );
  new EmpirePrices( parent, -1, Rect( Point( ( parent->width() - size.width() ) / 2,
                                                   ( parent->height() - size.height() ) / 2), size ), city );
}

Trade::Trade(PlayerCityPtr city, Widget* parent, int id )
: Base( parent, city, id ), _d( new Impl )
{
  setupUI( ":/gui/tradeadv.gui" );

  _d->city = city;
  _d->allgoods = city->statistic().goods.details( false );

  GET_DWIDGET_FROM_UI( _d, btnEmpireMap  )
  GET_DWIDGET_FROM_UI( _d, btnPrices )
  GET_DWIDGET_FROM_UI( _d, gbInfo )

  CONNECT( _d->btnEmpireMap, onClicked(), this, Trade::deleteLater );
  CONNECT( _d->btnPrices, onClicked(), _d.data(), Impl::showGoodsPriceWindow );

  _d->updateGoodsInfo();

  auto btnHelp = new TexturedButton( this, Point( 12, height() - 39), Size( 24 ), -1, config::id.menu.helpInf );
  CONNECT( btnHelp, onClicked(), this, Trade::_showHelp );
}

void Trade::draw(gfx::Engine& painter )
{
  if( !visible() )
    return;

  Window::draw( painter );
}

Signal0<>& Trade::onEmpireMapRequest() { return _d->btnEmpireMap->onClicked(); }

void Trade::_showHelp()
{
  DictionaryWindow::show( this, "trade_advisor" );
}

}//end namespace advisorwnd

}//end namespace gui
