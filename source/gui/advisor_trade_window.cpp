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
#include "good/goodhelper.hpp"
#include "pushbutton.hpp"
#include "label.hpp"
#include "game/resourcegroup.hpp"
#include "core/stringhelper.hpp"
#include "gfx/engine.hpp"
#include "core/gettext.hpp"
#include "groupbox.hpp"
#include "objects/factory.hpp"
#include "city/helper.hpp"
#include "city/trade_options.hpp"
#include "objects/warehouse.hpp"
#include "good/goodstore.hpp"
#include "texturedbutton.hpp"
#include "core/event.hpp"
#include "core/foreach.hpp"
#include "core/logger.hpp"
#include "image.hpp"
#include "objects/constants.hpp"

using namespace constants;
using namespace gfx;

namespace gui
{

class TradeGoodInfo : public PushButton
{
public:
  TradeGoodInfo( Widget* parent, const Rect& rect, Good::Type good, int qty, bool enable,
                 city::TradeOptions::Order trade, int tradeQty )
    : PushButton( parent, rect, "", -1, false, PushButton::noBackground )
  {
    _type = good;
    _qty = qty;
    _enable = enable;
    _tradeOrder = trade;
    _tradeQty = tradeQty;
    _goodPicture = GoodHelper::getPicture( _type );
    _goodName = GoodHelper::getName( _type );

    setFont( Font::create( FONT_2_WHITE ) );
  }

  virtual void draw(Engine &painter)
  {
    PushButton::draw( painter );

    painter.draw( _goodPicture, absoluteRect().lefttop() + Point( 15, 0) );
    painter.draw( _goodPicture, absoluteRect().righttop() - Point( 20 + _goodPicture.width(), 0 ) );
  }

  virtual void _updateTextPic()
  {
    PushButton::_updateTextPic();

    if( _textPictureRef() != 0 )
    {
      Font f = font( _state() );
      PictureRef& textPic = _textPictureRef();
      f.draw( *textPic, _goodName, 55, 0 );
      f.draw( *textPic, StringHelper::format( 0xff, "%d", _qty), 190, 0 );
      f.draw( *textPic, _enable ? "" : _("##disable##"), 260, 0 );

      std::string ruleName[] = { _("##import##"), "", _("##export##"), _("##stacking##") };
      std::string tradeStateText = ruleName[ _tradeOrder ];
      switch( _tradeOrder )
      {
      case city::TradeOptions::noTrade:
      case city::TradeOptions::stacking:
      case city::TradeOptions::importing:
        tradeStateText = ruleName[ _tradeOrder ];
      break;

      case city::TradeOptions::exporting:
        tradeStateText = StringHelper::format( 0xff, "%s %d", ruleName[ _tradeOrder ].c_str(), _tradeQty );
      break;

      default: break;
      }
      f.draw( *textPic, tradeStateText, 340, 0 );

      if( _state() == stHovered )
      {
     //   Decorator::draw( *background, Rect( 50, 0, width() - 50, height() ), Decorator::brownBorder, false );
      }
    }
  }

  Signal1<Good::Type>& onClickedA() { return _onClickedASignal; }

protected:
  virtual void _btnClicked()
  {
    PushButton::_btnClicked();

    oc3_emit _onClickedASignal( _type );
  }

private:
  int _qty;
  bool _enable;
  bool _stacking;
  city::TradeOptions::Order _tradeOrder;
  int _tradeQty;
  Good::Type _type;
  std::string _goodName;
  Picture _goodPicture;

oc3_signals private:
  Signal1<Good::Type> _onClickedASignal;
};

class EmpirePricesWindow : public Window
{
public:
  EmpirePricesWindow( Widget* parent, int id, const Rect& rectangle, PlayerCityPtr city  )
    : Window( parent, rectangle, "", id )
  {
    setupUI( ":/gui/empireprices.gui" );

    city::TradeOptions& ctrade = city->tradeOptions();
    Font font = Font::create( FONT_1 );
    Point startPos( 140, 50 );    
    for( int i=Good::wheat; i < Good::prettyWine; i++ )
    {
      if( i == Good::fish || i == Good::denaries)
      {
        continue;
      }

      Good::Type gtype = (Good::Type)i;
      Picture goodIcon = GoodHelper::getPicture( gtype );
      new Image( this, startPos, goodIcon );
      
      std::string priceStr = StringHelper::format( 0xff, "%d", ctrade.buyPrice( gtype ) );
      Label* lb = new Label( this, Rect( startPos + Point( 0, 34 ), Size( 24, 24 ) ), priceStr );
      lb->setFont( font );

      priceStr = StringHelper::format( 0xff, "%d", ctrade.sellPrice( gtype ) );
      lb = new Label( this, Rect( startPos + Point( 0, 58 ), Size( 24, 24 ) ), priceStr );
      lb->setFont( font );

      startPos += Point( 30, 0 );
    }
  }

  virtual void draw( gfx::Engine& painter )
  {
    if( !visible() )
      return;

    Window::draw( painter );
  }

  virtual bool onEvent(const NEvent& event)
  {
    if( event.EventType == sEventMouse && event.mouse.isRightPressed() )
    {
      deleteLater();
      return true;
    }

    return Window::onEvent( event );
  }
};

class AdvisorTradeWindow::Impl
{
public:
  PushButton* btnEmpireMap;
  PushButton* btnPrices; 
  GroupBox* gbInfo;
  PlayerCityPtr city;

  bool getWorkState( Good::Type gtype );
  int  getStackedGoodsQty( Good::Type gtype );
  void updateGoodsInfo();
  void showGoodOrderManageWindow( Good::Type type );
  void showGoodsPriceWindow();
};

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
      case city::TradeOptions::importing:
      case city::TradeOptions::noTrade:
      {
        btnDecrease->hide();
        btnIncrease->hide();

        Font f = font( _state() );
        std::string text = (order == city::TradeOptions::importing ? _("##trade_btn_import_text##") : _("##trade_btn_notrade_text##"));
        Rect textRect = f.getTextRect( text, Rect( Point( 0, 0), size() ), horizontalTextAlign(), verticalTextAlign() );
        f.draw( *_textPictureRef(), text, textRect.UpperLeftCorner );
      }
      break;

      case city::TradeOptions::exporting:
        {
          btnDecrease->show();
          btnIncrease->show();

          Font f = font( _state() );
          std::string text = _("##trade_btn_export_text##");
          Rect textRect = f.getTextRect( text, Rect( 0, 0, width() / 2, height() ), horizontalTextAlign(), verticalTextAlign() );
          f.draw( *_textPictureRef(), text, textRect.UpperLeftCorner, true );

          text = StringHelper::format( 0xff, "%d %s", goodsQty, _("##trade_btn_qty##") );
          textRect = f.getTextRect( text, Rect( width() / 2 + 24 * 2, 0, width(), height() ), horizontalTextAlign(), verticalTextAlign() );
          f.draw( *_textPictureRef(), text, textRect.UpperLeftCorner, true );
        }
      break;

      default: break;
    }
  }

  void setTradeState( city::TradeOptions::Order o, int qty )
  {
    order = o;
    goodsQty = qty;
    _resizeEvent();
  }
  
  city::TradeOptions::Order order;
  int goodsQty;
  TexturedButton* btnDecrease;
  TexturedButton* btnIncrease;
};

class GoodOrderManageWindow : public Widget
{
public:
  GoodOrderManageWindow( Widget* parent, const Rect& rectangle, PlayerCityPtr city, Good::Type type, int stackedGoods )
    : Widget( parent, -1, rectangle )
  {
    _city = city;
    _type = type;
    _background.reset( Picture::create( size() ) );
    Decorator::draw( *_background, Rect( Point( 0, 0 ), size() ), Decorator::whiteFrame );

    Picture iconGood = GoodHelper::getPicture( type );
    _background->draw( iconGood, Point( 10, 10 ) );

    Label* lbTitle = new Label( this, Rect( 40, 10, width() - 10, 10 + 30), GoodHelper::getName( type ) );
    lbTitle->setFont( Font::create( FONT_5 ) );

    _lbIndustryInfo = new Label( this, Rect( 40, 40, width() - 10, 40 + 20 ) );

    std::string text = StringHelper::format( 0xff, "%d %s", stackedGoods, _("##qty_stacked_in_city_warehouse##") );
    /*Label* lbStacked = */new Label( this, Rect( 40, 60, width() - 10, 60 + 20 ), text );

    _btnTradeState = new TradeStateButton( this, Rect( 50, 85, width() - 60, 85 + 30), -1 );
    _btnIndustryState = new PushButton( this, Rect( 50, 125, width() - 60, 125 + 30), "", -1, false, PushButton::whiteBorderUp );
    _btnStackingState = new PushButton( this, Rect( 50, 160, width() - 60, 160 + 50), "", -1, false, PushButton::whiteBorderUp );

    TexturedButton* btnExit = new TexturedButton( this, Point( width() - 34, height() - 34 ), Size( 24 ), -1, ResourceMenu::exitInfBtnPicId );
    /*TexturedButton* btnHelp = */new TexturedButton( this, Point( 11, height() - 34 ), Size( 24 ), -1, ResourceMenu::helpInfBtnPicId );

    updateTradeState();
    updateIndustryState();
    updateStackingState();

    CONNECT( btnExit, onClicked(), this, GoodOrderManageWindow::deleteLater );
    CONNECT( _btnTradeState, onClicked(), this, GoodOrderManageWindow::changeTradeState );
    CONNECT( _btnTradeState->btnIncrease, onClicked(), this, GoodOrderManageWindow::increaseQty );
    CONNECT( _btnTradeState->btnDecrease, onClicked(), this, GoodOrderManageWindow::decreaseQty );
    CONNECT( _btnIndustryState, onClicked(), this, GoodOrderManageWindow::toggleIndustryEnable );
    CONNECT( _btnStackingState, onClicked(), this, GoodOrderManageWindow::toggleStackingGoods );
  }

  void draw( gfx::Engine& painter )
  {
    if( !visible() )
      return;

    painter.draw( *_background, screenLeft(), screenTop() );

    Widget::draw( painter );
  }

  void increaseQty()
  {
    city::TradeOptions& ctrade = _city->tradeOptions();
    ctrade.setExportLimit( _type, math::clamp<unsigned int>( ctrade.exportLimit( _type )+1, 0, 999 ) );
    updateTradeState();
  }

  void decreaseQty()
  {
    city::TradeOptions& ctrade = _city->tradeOptions();
    ctrade.setExportLimit( _type, math::clamp<unsigned int>( ctrade.exportLimit( _type )-1, 0, 999 ) );
    updateTradeState();
  }

  void updateTradeState()
  {
    city::TradeOptions& ctrade = _city->tradeOptions();
    city::TradeOptions::Order order = ctrade.getOrder( _type );
    int qty = ctrade.exportLimit( _type );
    _btnTradeState->setTradeState( order, qty );
  }

  void changeTradeState()
  {
    _city->tradeOptions().switchOrder( _type );
    updateTradeState();
    oc3_emit _onOrderChangedSignal();
  }

  bool isIndustryEnabled()
  {
    city::Helper helper( _city );
    //if any factory work in city, that industry work too
    bool anyFactoryWork = false;
    FactoryList factories = helper.getProducers<Factory>( _type );
    foreach( factory, factories )
    {
      anyFactoryWork |= (*factory)->isActive();
    }

    return factories.empty() ? true : anyFactoryWork;
  }

  void updateIndustryState()
  {
    bool industryActive = _city->tradeOptions().isVendor( _type );
    _btnIndustryState->setVisible( industryActive );

    if( !industryActive )
    {
      return;
    }

    city::Helper helper( _city );
    int workFactoryCount=0, idleFactoryCount=0;

    FactoryList factories = helper.getProducers<Factory>( _type );
    foreach( factory, factories )
    {
      ( (*factory)->standIdle() ? idleFactoryCount : workFactoryCount ) += 1;
    }

    std::string text = StringHelper::format( 0xff, "%d %s, %d %s", workFactoryCount, _("##work##"), 
                                                                   idleFactoryCount, _("##idle_factory_in_city##") );
    _lbIndustryInfo->setText( text );

    bool industryEnabled = isIndustryEnabled();
    _btnIndustryState->setText( industryEnabled ? _("##industry_enabled##") : _("##industry_disabled##") );
  }

  void toggleIndustryEnable()
  {
    city::Helper helper( _city );

    bool industryEnabled = isIndustryEnabled();
    //up or down all factory for this industry
    FactoryList factories = helper.getProducers<Factory>( _type );
    foreach( factory, factories ) { (*factory)->setActive( !industryEnabled ); }

    updateIndustryState();
    oc3_emit _onOrderChangedSignal();
  }

  void toggleStackingGoods()
  {
    bool isStacking = _city->tradeOptions().isGoodsStacking( _type );
    _city->tradeOptions().setStackMode( _type, !isStacking );

    updateStackingState();
    oc3_emit _onOrderChangedSignal();
  }

  void updateStackingState()
  {
    bool isStacking = _city->tradeOptions().isGoodsStacking( _type );
    std::string text;
    if( isStacking )
    {
      text = StringHelper::format( 0xff, "%s %s", _("##stacking_resource##"), _("##click_here_that_use_it##") );
    }
    else
    {
      text = StringHelper::format( 0xff, "%s %s", _("##use_and_trade_resource##"), _("##click_here_that_stacking##") );
    }

    _btnStackingState->setText( text );
  }

oc3_signals public:
  Signal0<>& onOrderChanged() { return _onOrderChangedSignal; }

private:
  PlayerCityPtr _city;
  Good::Type _type;
  PictureRef _background;
  TradeStateButton* _btnTradeState;
  PushButton* _btnIndustryState;
  Label* _lbIndustryInfo;
  PushButton* _btnStackingState;

oc3_signals private:
  Signal0<> _onOrderChangedSignal;
};

void AdvisorTradeWindow::Impl::updateGoodsInfo()
{
  if( !gbInfo )
    return;

  Widget::Widgets children = gbInfo->children();

  foreach( child, children ) { (*child)->deleteLater(); }

  Point startDraw( 0, 5 );
  Size btnSize( gbInfo->width(), 20 );
  city::TradeOptions& copt = city->tradeOptions();
  for( int i=Good::wheat, indexOffset=0; i < Good::goodCount; i++ )
  {
    Good::Type gtype = Good::Type( i );

    city::TradeOptions::Order tradeState = copt.getOrder( gtype );
    if( tradeState == city::TradeOptions::disabled )
    {
      continue;
    }

    int stackedQty = getStackedGoodsQty( gtype );
    bool workState = getWorkState( gtype );
    int tradeQty = copt.exportLimit( gtype );
    
    TradeGoodInfo* btn = new TradeGoodInfo( gbInfo, Rect( startDraw + Point( 0, btnSize.height()) * indexOffset, btnSize ),
                                            gtype, stackedQty, workState, tradeState, tradeQty );
    indexOffset++;
    CONNECT( btn, onClickedA(), this, Impl::showGoodOrderManageWindow );
  }
}

bool AdvisorTradeWindow::Impl::getWorkState(Good::Type gtype )
{
  city::Helper helper( city );

  bool industryActive = false;
  FactoryList producers = helper.getProducers<Factory>( gtype );

  foreach( it, producers ) { industryActive |= (*it)->isActive(); }

  return producers.empty() ? true : industryActive;
}

int AdvisorTradeWindow::Impl::getStackedGoodsQty( Good::Type gtype )
{
  city::Helper helper( city );

  int goodsQty = 0;
  WarehouseList warehouses = helper.find< Warehouse >( building::warehouse );
  foreach( it, warehouses ) { goodsQty += (*it)->store().qty( gtype ); }

  return goodsQty;
}

void AdvisorTradeWindow::Impl::showGoodOrderManageWindow(Good::Type type )
{
  Widget* parent = gbInfo->parent();
  int stackedGoods = getStackedGoodsQty( type ) ;
  GoodOrderManageWindow* wnd = new GoodOrderManageWindow( parent, Rect( 50, 130, parent->width() - 45, parent->height() -60 ), 
                                                          city, type, stackedGoods );

  CONNECT( wnd, onOrderChanged(), this, Impl::updateGoodsInfo );
}

void AdvisorTradeWindow::Impl::showGoodsPriceWindow()
{
  Widget* parent = gbInfo->parent();
  Size size( 610, 180 );
  new EmpirePricesWindow( parent, -1, Rect( Point( ( parent->width() - size.width() ) / 2,
                                                   ( parent->height() - size.height() ) / 2), size ), city );
}

AdvisorTradeWindow::AdvisorTradeWindow(PlayerCityPtr city, Widget* parent, int id )
: Window( parent, Rect( 0, 0, 640, 432 ), "", id ), _d( new Impl )
{
  setupUI( ":/gui/tradeadv.gui" );
  setPosition( Point( (parent->width() - 640 )/2, parent->height() / 2 - 242 ) );

  _d->city = city;

  _d->btnEmpireMap = findChildA<PushButton*>( "btnEmpireMap", true, this );
  _d->btnPrices = findChildA<PushButton*>( "btnPrices", true, this );
  _d->gbInfo = findChildA<GroupBox*>( "gbInfo", true, this );

  CONNECT( _d->btnEmpireMap, onClicked(), this, AdvisorTradeWindow::deleteLater );
  CONNECT( _d->btnPrices, onClicked(), _d.data(), Impl::showGoodsPriceWindow );

  _d->updateGoodsInfo();
}

void AdvisorTradeWindow::draw(gfx::Engine& painter )
{
  if( !visible() )
    return;

  Window::draw( painter );
}

Signal0<>& AdvisorTradeWindow::onEmpireMapRequest()
{
  return _d->btnEmpireMap->onClicked();
}

}//end namespace gui
