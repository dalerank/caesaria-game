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

#include "goodordermanage.hpp"
#include "city/trade_options.hpp"
#include "texturedbutton.hpp"
#include "core/gettext.hpp"
#include "good/goodhelper.hpp"
#include "label.hpp"
#include "objects/factory.hpp"
#include "city/helper.hpp"
#include "gfx/engine.hpp"
#include "widget_helper.hpp"
#include "core/logger.hpp"
#include "core/utils.hpp"

using namespace constants;
using namespace gfx;
using namespace city;

namespace gui
{

namespace advisorwnd
{

class TradeStateButton : public PushButton
{
public:
  TradeStateButton( Widget* parent, const Rect& rectangle, int id )
    : PushButton( parent, rectangle, "", id, false, PushButton::whiteBorderUp )
  {
    btnDecrease = new TexturedButton( this, Point( 220, 3 ), Size( 24 ), -1, 601 );
    btnIncrease = new TexturedButton( this, Point( 220 + 24, 3 ), Size( 24 ), -1, 605 );
    btnDecrease->hide();
    btnIncrease->hide();

    btnDecrease->setTooltipText( _("##export_btn_tooltip##") );
    btnIncrease->setTooltipText( _("##export_btn_tooltip##") );
  }

  virtual void _updateTextPic()
  {
    PushButton::_updateTextPic();

    switch( order )
    {
      case trade::noTrade:
      {
        btnDecrease->hide();
        btnIncrease->hide();

        Font f = font( _state() );
        std::string text = _("##trade_btn_notrade_text##");
        Rect textRect = f.getTextRect( text, Rect( Point( 0, 0), size() ), horizontalTextAlign(), verticalTextAlign() );
        f.draw( *_textPictureRef(), text, textRect.UpperLeftCorner );
      }
      break;

      case trade::importing:
      case trade::exporting:
      {
        btnDecrease->show();
        btnIncrease->show();

        Font f = font( _state() );
        std::string text = (order == trade::importing ? "##trade_btn_import_text##" : "##trade_btn_export_text##");
        Rect textRect = f.getTextRect( _(text), Rect( 0, 0, width() / 2, height() ), horizontalTextAlign(), verticalTextAlign() );
        f.draw( *_textPictureRef(), _(text), textRect.UpperLeftCorner, true );

        text = utils::format( 0xff, "%d %s", goodsQty, _("##trade_btn_qty##") );
        textRect = f.getTextRect( text, Rect( width() / 2 + 24 * 2, 0, width(), height() ), horizontalTextAlign(), verticalTextAlign() );
        f.draw( *_textPictureRef(), text, textRect.UpperLeftCorner, true );
      }
      break;

      default: break;
    }
  }

  void update()
  {
    _resizeEvent();
  }

  void setTradeState( trade::Order o, int qty )
  {
    order = o;
    goodsQty = qty;
    _resizeEvent();
  }

  trade::Order order;
  int goodsQty;
  TexturedButton* btnDecrease;
  TexturedButton* btnIncrease;
};

class GoodOrderManageWindow::Impl
{
public:
  PlayerCityPtr city;
  good::Product type;
  TradeStateButton* btnTradeState;
  PushButton* btnIndustryState;
  Label* lbIndustryInfo;
  Picture icon;
  GoodOrderManageWindow::GoodMode gmode;
  PushButton* btnStackingState;

signals public:
  Signal0<> onOrderChangedSignal;
};

GoodOrderManageWindow::GoodOrderManageWindow(Widget *parent, const Rect &rectangle, PlayerCityPtr city,
                                             good::Product type, int stackedGoods, GoodMode gmode )
  : Window( parent, rectangle, "" ), _d( new Impl )
{  
  _d->city = city;
  _d->type = type;
  _d->gmode = gmode;

  setupUI( ":/gui/goodorder.gui" );

  _d->icon = good::Helper::picture( type );

  INIT_WIDGET_FROM_UI( Label*, lbTitle )
  INIT_WIDGET_FROM_UI( Label*, lbStackedQty )
  INIT_WIDGET_FROM_UI( TexturedButton*, btnExit )
  GET_DWIDGET_FROM_UI( _d, lbIndustryInfo )
  GET_DWIDGET_FROM_UI( _d, btnIndustryState )
  GET_DWIDGET_FROM_UI( _d, btnStackingState )

  if( lbTitle ) lbTitle->setText( _( good::Helper::name( type ) ) );
  if( lbStackedQty )
  {
    std::string text = utils::format( 0xff, "%d %s", stackedGoods, _("##qty_stacked_in_city_warehouse##") );
    lbStackedQty->setText( text );
  }

  _d->btnTradeState = new TradeStateButton( this, Rect( 50, 90, width() - 60, 90 + 30), -1 );
  if( gmode == gmUnknown )
  {
    _d->btnTradeState->setTradeState( trade::noTrade, 0 );
    _d->btnTradeState->setEnabled( false );
  }

  updateTradeState();
  updateIndustryState();
  updateStackingState();

  CONNECT( btnExit, onClicked(), this, GoodOrderManageWindow::deleteLater );
  CONNECT( _d->btnTradeState, onClicked(), this, GoodOrderManageWindow::changeTradeState );
  CONNECT( _d->btnTradeState->btnIncrease, onClicked(), this, GoodOrderManageWindow::increaseQty );
  CONNECT( _d->btnTradeState->btnDecrease, onClicked(), this, GoodOrderManageWindow::decreaseQty );
  CONNECT( _d->btnIndustryState, onClicked(), this, GoodOrderManageWindow::toggleIndustryEnable );
  CONNECT( _d->btnStackingState, onClicked(), this, GoodOrderManageWindow::toggleStackingGoods );

  setModal();
}

void GoodOrderManageWindow::draw(Engine &painter)
{
  if( !visible() )
    return;

  Window::draw( painter );

  painter.draw( _d->icon, absoluteRect().lefttop() + Point( 10, 10 ) );
}

void GoodOrderManageWindow::increaseQty() { _changeTradeLimit( +1 ); }

void GoodOrderManageWindow::decreaseQty() { _changeTradeLimit( -1 ); }

void GoodOrderManageWindow::updateTradeState()
{
  trade::Options& ctrade = _d->city->tradeOptions();
  trade::Order order = ctrade.getOrder( _d->type );
  int qty = ctrade.tradeLimit( order, _d->type );
  _d->btnTradeState->setTradeState( order, qty );
}

void GoodOrderManageWindow::changeTradeState()
{
  trade::Options& trOpts = _d->city->tradeOptions();
  if( _d->gmode == gmImport )
  {
    trade::Order order = trOpts.getOrder( _d->type );
    trOpts.setOrder( _d->type, order == trade::importing
                                  ? trade::noTrade
                                  : trade::importing );
  }
  else
  {
    trOpts.switchOrder( _d->type );
  }

  updateTradeState();
  emit _d->onOrderChangedSignal();
}

bool GoodOrderManageWindow::isIndustryEnabled()
{
  city::Helper helper( _d->city );
  //if any factory work in city, that industry work too
  bool anyFactoryWork = false;
  FactoryList factories = helper.getProducers<Factory>( _d->type );
  foreach( factory, factories )
  {
    anyFactoryWork |= (*factory)->isActive();
  }

  return factories.empty() ? true : anyFactoryWork;
}

void GoodOrderManageWindow::updateIndustryState()
{
  city::Helper helper( _d->city );
  int workFactoryCount=0, idleFactoryCount=0;
  FactoryList factories = helper.getProducers<Factory>( _d->type );
  foreach( factory, factories )
  {
    ( (*factory)->standIdle() ? idleFactoryCount : workFactoryCount ) += 1;
  }

  //bool industryActive = _d->city->tradeOptions().isVendor( _d->type );
  if( factories.empty() )
  {
    _d->btnIndustryState->setEnabled( false );
    _d->btnIndustryState->setBackgroundStyle( PushButton::noBackground );
    _d->btnIndustryState->setText( _("##no_industries_in_city##" ) );
    return;
  }  

  std::string postfixWork = (workFactoryCount%10 == 1) ? "##working_industry##" : "##working_industries##";
  std::string postfixIdle = (workFactoryCount%10 == 1) ? "##idle_factory_in_city##" : "##idle_factories_in_city##";

  std::string text = utils::format( 0xff, "%d %s\n%d %s", workFactoryCount, _(postfixWork),
                                           idleFactoryCount, _(postfixIdle) );
  _d->lbIndustryInfo->setText( text );

  bool industryEnabled = isIndustryEnabled();
  _d->btnIndustryState->setText( industryEnabled ? _("##industry_enabled##") : _("##industry_disabled##") );
}

void GoodOrderManageWindow::toggleIndustryEnable()
{
  city::Helper helper( _d->city );

  bool industryEnabled = isIndustryEnabled();
  //up or down all factory for this industry
  FactoryList factories = helper.getProducers<Factory>( _d->type );
  foreach( factory, factories ) { (*factory)->setActive( !industryEnabled ); }

  updateIndustryState();
  emit _d->onOrderChangedSignal();
}

void GoodOrderManageWindow::toggleStackingGoods()
{
  bool isStacking = _d->city->tradeOptions().isGoodsStacking( _d->type );
  _d->city->tradeOptions().setStackMode( _d->type, !isStacking );

  updateStackingState();
  emit _d->onOrderChangedSignal();
}

void GoodOrderManageWindow::updateStackingState()
{
  bool isStacking = _d->city->tradeOptions().isGoodsStacking( _d->type );
  std::string text;
  if( isStacking )
  {
    text = utils::format( 0xff, "%s %s", _("##stacking_resource##"), _("##click_here_that_use_it##") );
  }
  else
  {
    text = utils::format( 0xff, "%s %s", _("##use_and_trade_resource##"), _("##click_here_that_stacking##") );
  }

  _d->btnStackingState->setText( text );
}

Signal0<> &GoodOrderManageWindow::onOrderChanged() { return _d->onOrderChangedSignal; }

void GoodOrderManageWindow::_changeTradeLimit(int value)
{
  trade::Options& ctrade = _d->city->tradeOptions();

  trade::Order state = _d->btnTradeState->order;
  if( state == trade::importing ||
      state == trade::exporting )
  {
    unsigned int limit = ctrade.tradeLimit( state, _d->type );
    limit = math::clamp<int>( limit+value, 0, 999 );
    ctrade.setTradeLimit( state, _d->type, limit );
  }
  updateTradeState();
}

}

}//end namespace gui
