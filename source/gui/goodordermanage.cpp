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
#include "good/helper.hpp"
#include "label.hpp"
#include "objects/factory.hpp"
#include "gfx/engine.hpp"
#include "widget_helper.hpp"
#include "core/logger.hpp"
#include "core/utils.hpp"
#include "core/event.hpp"
#include "city/statistic.hpp"
#include "widgetescapecloser.hpp"
#include "stretch_layout.hpp"
#include "multilinebutton.hpp"
#include "dialogbox.hpp"

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
    btnDecrease = &add<TexturedButton>( Point( 220, 3 ), Size( 24 ), -1, 601 );
    btnIncrease = &add<TexturedButton>(  Point( 220 + 24, 3 ), Size( 24 ), -1, 605 );
    btnDecrease->hide();
    btnIncrease->hide();

    btnDecrease->setTooltipText( _("##export_btn_tooltip##") );
    btnIncrease->setTooltipText( _("##export_btn_tooltip##") );
  }

  virtual void _updateTexture()
  {
    PushButton::_updateTexture();

    switch( order )
    {
      case trade::noTrade:
      {
        btnDecrease->hide();
        btnIncrease->hide();

        Font f = font( _state() );
        std::string text = _("##trade_btn_notrade_text##");
        Rect textRect = f.getTextRect( text, Rect( Point( 0, 0), size() ), horizontalTextAlign(), verticalTextAlign() );
        f.draw( _textPicture(), text, textRect._lefttop );
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
        f.draw( _textPicture(), _(text), textRect._lefttop, true );

        text = fmt::format( "{} {}", goodsQty, _("##trade_btn_qty##") );
        textRect = f.getTextRect( text, Rect( width() / 2 + 24 * 2, 0, width(), height() ), horizontalTextAlign(), verticalTextAlign() );
        f.draw( _textPicture(), text, textRect._lefttop, true );
      }
      break;

      default: break;
    }
  }

  void update()
  {
    _finalizeResize();
  }

  void setTradeState( trade::Order o, int qty )
  {
    order = o;
    goodsQty = qty;
    _finalizeResize();
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
  MultilineButton* btnStackingState;

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
  WidgetClose::insertTo( this, KEY_RBUTTON );

  _d->icon = good::Info( type ).picture();

  INIT_WIDGET_FROM_UI( Label*, lbTitle )
  INIT_WIDGET_FROM_UI( Label*, lbStackedQty )
  GET_DWIDGET_FROM_UI( _d, lbIndustryInfo )
  GET_DWIDGET_FROM_UI( _d, btnIndustryState )
  GET_DWIDGET_FROM_UI( _d, btnStackingState )

  if( lbTitle ) lbTitle->setText( _( good::Helper::name( type ) ) );
  if( lbStackedQty )
  {
    std::string text = utils::format( 0xff, "%d %s", stackedGoods, _("##qty_stacked_in_city_warehouse##") );
    lbStackedQty->setText( text );
  }

  _d->btnTradeState = &add<TradeStateButton>( Rect( 50, 90, width() - 60, 90 + 30), -1 );

  updateTradeState();
  updateIndustryState();
  updateStackingState();

  LINK_WIDGET_LOCAL_ACTION( TexturedButton*, btnExit, onClicked(), GoodOrderManageWindow::deleteLater );
  CONNECT_LOCAL( _d->btnTradeState, onClicked(),              GoodOrderManageWindow::changeTradeState );
  CONNECT_LOCAL( _d->btnTradeState->btnIncrease, onClicked(), GoodOrderManageWindow::increaseQty );
  CONNECT_LOCAL( _d->btnTradeState->btnDecrease, onClicked(), GoodOrderManageWindow::decreaseQty );
  CONNECT_LOCAL( _d->btnIndustryState, onClicked(),           GoodOrderManageWindow::toggleIndustryEnable );
  CONNECT_LOCAL( _d->btnStackingState, onClicked(),           GoodOrderManageWindow::toggleStackingGoods );

  moveTo( Widget::parentCenter );
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
  int qty = ctrade.tradeLimit( order, _d->type ).ivalue();
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
  //if any factory work in city, that industry work too
  bool anyFactoryWork = false;
  FactoryList factories = _d->city->statistic().objects.producers<Factory>( _d->type );
  for( auto factory : factories )
  {
    anyFactoryWork |= factory->isActive();
  }

  return factories.empty() ? true : anyFactoryWork;
}

void GoodOrderManageWindow::updateIndustryState()
{
  int workFactoryCount=0, idleFactoryCount=0;
  FactoryList factories = _d->city->statistic().objects.producers<Factory>( _d->type );
  for( auto factory : factories )
  {
    ( factory->standIdle() ? idleFactoryCount : workFactoryCount ) += 1;
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
  bool industryEnabled = isIndustryEnabled();
  //up or down all factory for this industry
  FactoryList factories = _d->city->statistic().objects.producers<Factory>( _d->type );
  for( auto factory : factories )
    factory->setActive( !industryEnabled );

  if( !industryEnabled )
  {
    dialog::Confirmation( ui(), "Note", "Do you want fire workers from industry?",
                          makeDelegate( this, &GoodOrderManageWindow::_fireWorkers ) );
  }

  updateIndustryState();
  emit _d->onOrderChangedSignal();
}

void GoodOrderManageWindow::toggleStackingGoods()
{
  bool isStacking = _d->city->tradeOptions().isStacking( _d->type );
  _d->city->tradeOptions().setStackMode( _d->type, !isStacking );

  updateStackingState();
  emit _d->onOrderChangedSignal();
}

void GoodOrderManageWindow::updateStackingState()
{
  bool isStacking = _d->city->tradeOptions().isStacking( _d->type );
  StringArray text;
  if( isStacking )
  {
    text << _("##stacking_resource##");
    text << _("##click_here_that_use_it##");
  }
  else
  {
    text << _("##use_and_trade_resource##");
    text << _("##click_here_that_stacking##");
  }

  _d->btnStackingState->setText( text );
  _d->btnStackingState->setLineFont( 1, Font::create( FONT_0 ) );
}

Signal0<>& GoodOrderManageWindow::onOrderChanged() { return _d->onOrderChangedSignal; }

void GoodOrderManageWindow::_changeTradeLimit(int value)
{
  trade::Options& ctrade = _d->city->tradeOptions();

  trade::Order state = _d->btnTradeState->order;
  if( state == trade::importing ||
      state == trade::exporting )
  {
    unsigned int limit = ctrade.tradeLimit( state, _d->type ).ivalue();
    limit = math::clamp<int>( limit+value, 0, 999 );
    ctrade.setTradeLimit( state, _d->type, metric::Unit::fromValue( limit ) );
  }
  updateTradeState();
  emit _d->onOrderChangedSignal();
}

void GoodOrderManageWindow::_fireWorkers()
{
  FactoryList factories = _d->city->statistic().objects.producers<Factory>( _d->type );
  for( auto factory : factories )
    factory->removeWorkers( factory->numberWorkers() );
}

}//end namespace advisorwnd

}//end namespace gui
