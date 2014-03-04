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
#include "objects/constants.hpp"

using namespace constants;

namespace gui
{

class TradeGoodInfo : public PushButton
{
public:
  TradeGoodInfo( Widget* parent, const Rect& rect, Good::Type good, int qty, bool enable,
                 CityTradeOptions::Order trade, int tradeQty )
    : PushButton( parent, rect, "" )
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

  virtual void _updateTexture( ElementState state )
  {
    PushButton::_updateTexture( state );

    PictureRef& background = _getBackground( state );
    background->fill( 0x00ffffff, Rect( 0, 0, 0, 0) );
    background->draw( _goodPicture, 15, 0, false );
    background->draw( _goodPicture, width() - 20 - _goodPicture.getWidth(), 0, false );

    if( _getTextPicture( state ) != 0 )
    {
      Font font = getFont( state );    
      PictureRef& textPic = _getTextPicture( state );
      font.draw( *textPic, _goodName, 55, 0 );   
      font.draw( *textPic, StringHelper::format( 0xff, "%d", _qty), 190, 0 );
      font.draw( *textPic, _enable ? "" : _("##disable##"), 260, 0 );

      std::string ruleName[] = { _("##import##"), "", _("##export##"), _("##stacking##") };
      std::string tradeStateText = ruleName[ _tradeOrder ];
      switch( _tradeOrder )
      {
      case CityTradeOptions::noTrade:
      case CityTradeOptions::stacking:
      case CityTradeOptions::importing: 
        tradeStateText = ruleName[ _tradeOrder ];
      break;
      
      case CityTradeOptions::exporting:
        tradeStateText = StringHelper::format( 0xff, "%s %d", ruleName[ _tradeOrder ].c_str(), _tradeQty );
      break;

      default: break;
      }
      font.draw( *textPic, tradeStateText, 340, 0 );

      if( state == stHovered ) 
      {
        PictureDecorator::draw( *background, Rect( 50, 0, width() - 50, getHeight() ), PictureDecorator::brownBorder, false );
      }
    }
  }

  Signal1<Good::Type>& onClickedA() { return _onClickedASignal; }

protected:
  void _btnClicked()
  {
    PushButton::_btnClicked();

    _onClickedASignal.emit( _type );
  }

private:
  int _qty;
  bool _enable;
  bool _stacking;
  CityTradeOptions::Order _tradeOrder;
  int _tradeQty;
  Good::Type _type;
  std::string _goodName;
  Picture _goodPicture;

oc3_signals private:
  Signal1<Good::Type> _onClickedASignal;
};

class EmpirePricesWindow : public Widget
{
public:
  EmpirePricesWindow( Widget* parent, int id, const Rect& rectangle, PlayerCityPtr city  )
    : Widget( parent, id, rectangle )
  {
    background.reset( Picture::create( getSize() ) );
    PictureDecorator::draw( *background, Rect( Point( 0, 0 ), getSize() ), PictureDecorator::whiteFrame );

    Font font = Font::create( FONT_3 );
    font.draw( *background, _("##rome_prices##"), Point( 10, 10 ), false );

    CityTradeOptions& ctrade = city->getTradeOptions();
    font = Font::create( FONT_1 );
    Point startPos( 140, 50 );
    for( int i=Good::wheat; i < Good::goodCount; i++ )
    {
      if( i == Good::fish || i == Good::denaries)
      {
        continue;
      }

      Good::Type gtype = (Good::Type)i;
      const Picture& goodIcon = GoodHelper::getPicture( gtype );
      background->draw( goodIcon, startPos );
      
      std::string priceStr = StringHelper::format( 0xff, "%d", ctrade.getBuyPrice( gtype ) );      
      font.draw( *background, priceStr, startPos + Point( 0, 34 ), false );

      priceStr = StringHelper::format( 0xff, "%d", ctrade.getSellPrice( gtype ) );
      font.draw( *background, priceStr, startPos + Point( 0, 58 ), false );

      startPos += Point( 30, 0 );
    }

    font = Font::create( FONT_2 );
    font.draw( *background, _("##buy_price##"), Point( 10, 84 ), false );
    font.draw( *background, _("##sell_price##"), Point( 10, 108 ), false );

    font.draw( *background, _("##click_rmb_for_exit##"), Point( 140, getHeight() - 30 ), false ); 
  }

  virtual void draw( GfxEngine& painter )
  {
    if( !isVisible() )
      return;

    painter.drawPicture( *background, getAbsoluteRect().UpperLeftCorner );

    Widget::draw( painter );
  }

  virtual bool onEvent(const NEvent& event)
  {
    if( event.EventType == sEventMouse && event.mouse.isRightPressed() )
    {
      deleteLater();
      return true;
    }

    return Widget::onEvent( event );
  }

  PictureRef background;
};

class AdvisorTradeWindow::Impl
{
public:
  PictureRef background;
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
  }

  virtual void _updateTexture( ElementState state )
  {
    PushButton::_updateTexture( state );

    switch( order )
    {
      case CityTradeOptions::importing:
      case CityTradeOptions::noTrade:
      {
        btnDecrease->hide();
        btnIncrease->hide();

        Font font = getFont( state );        
        std::string text = (order == CityTradeOptions::importing ? _("##trade_btn_import_text##") : _("##trade_btn_notrade_text##"));
        Rect textRect = font.calculateTextRect( text, Rect( Point( 0, 0), getSize() ), getHorizontalTextAlign(), getVerticalTextAlign() );
        font.draw( *_getTextPicture( state ), text, textRect.UpperLeftCorner );
      }
      break;

      case CityTradeOptions::exporting:
        {
          btnDecrease->show();
          btnIncrease->show();

          Font font = getFont( state );
          std::string text = _("##trade_btn_export_text##");
          Rect textRect = font.calculateTextRect( text, Rect( 0, 0, width() / 2, getHeight() ), getHorizontalTextAlign(), getVerticalTextAlign() );
          font.draw( *_getTextPicture( state ), text, textRect.UpperLeftCorner, true );

          text = StringHelper::format( 0xff, "%d %s", goodsQty, _("##trade_btn_qty##") );
          textRect = font.calculateTextRect( text, Rect( width() / 2 + 24 * 2, 0, width(), getHeight() ), getHorizontalTextAlign(), getVerticalTextAlign() );
          font.draw( *_getTextPicture( state ), text, textRect.UpperLeftCorner, true );
        }
      break;

      default: break;
    }
  }

  void setTradeState( CityTradeOptions::Order o, int qty )
  {
    order = o;
    goodsQty = qty;
    _resizeEvent();
  }
  
  CityTradeOptions::Order order;
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
    _background.reset( Picture::create( getSize() ) );
    PictureDecorator::draw( *_background, Rect( Point( 0, 0 ), getSize() ), PictureDecorator::whiteFrame );

    const Picture& iconGood = GoodHelper::getPicture( type );
    _background->draw( iconGood, Point( 10, 10 ) );

    Label* lbTitle = new Label( this, Rect( 40, 10, width() - 10, 10 + 30), GoodHelper::getName( type ) );
    lbTitle->setFont( Font::create( FONT_3 ) );

    _lbIndustryInfo = new Label( this, Rect( 40, 40, width() - 10, 40 + 20 ) );

    std::string text = StringHelper::format( 0xff, "%d %s", stackedGoods, _("##qty_stacked_in_city_warehouse##") );
    /*Label* lbStacked = */new Label( this, Rect( 40, 60, width() - 10, 60 + 20 ), text );

    _btnTradeState = new TradeStateButton( this, Rect( 50, 85, width() - 60, 85 + 30), -1 );
    _btnIndustryState = new PushButton( this, Rect( 50, 125, width() - 60, 125 + 30), "", -1, false, PushButton::whiteBorderUp );
    _btnStackingState = new PushButton( this, Rect( 50, 160, width() - 60, 160 + 50), "", -1, false, PushButton::whiteBorderUp );

    TexturedButton* btnExit = new TexturedButton( this, Point( width() - 34, getHeight() - 34 ), Size( 24 ), -1, ResourceMenu::exitInfBtnPicId );
    /*TexturedButton* btnHelp = */new TexturedButton( this, Point( 11, getHeight() - 34 ), Size( 24 ), -1, ResourceMenu::helpInfBtnPicId );

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

  void draw( GfxEngine& painter )
  {
    if( !isVisible() )
      return;

    painter.drawPicture( *_background, screenLeft(), getScreenTop() );

    Widget::draw( painter );
  }

  void increaseQty()
  {
    CityTradeOptions& ctrade = _city->getTradeOptions();
    ctrade.setExportLimit( _type, math::clamp( ctrade.getExportLimit( _type )+1, 0, 999 ) );
    updateTradeState();
  }

  void decreaseQty()
  {
    CityTradeOptions& ctrade = _city->getTradeOptions();
    ctrade.setExportLimit( _type, math::clamp( ctrade.getExportLimit( _type )-1, 0, 999 ) );
    updateTradeState();
  }

  void updateTradeState()
  {
    CityTradeOptions& ctrade = _city->getTradeOptions();
    CityTradeOptions::Order order = ctrade.getOrder( _type );
    int qty = ctrade.getExportLimit( _type );
    _btnTradeState->setTradeState( order, qty );
  }

  void changeTradeState()
  {
    _city->getTradeOptions().switchOrder( _type );
    updateTradeState();
    _onOrderChangedSignal.emit();
  }

  bool isIndustryEnabled()
  {
    CityHelper helper( _city );
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
    bool industryActive = _city->getTradeOptions().isVendor( _type );
    _btnIndustryState->setVisible( industryActive );

    if( !industryActive )
    {
      return;
    }

    CityHelper helper( _city );
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
    CityHelper helper( _city );

    bool industryEnabled = isIndustryEnabled();
    //up or down all factory for this industry
    FactoryList factories = helper.getProducers<Factory>( _type );
    foreach( factory, factories ) { (*factory)->setActive( !industryEnabled ); }

    updateIndustryState();
    _onOrderChangedSignal.emit();
  }

  void toggleStackingGoods()
  {
    bool isStacking = _city->getTradeOptions().isGoodsStacking( _type );
    _city->getTradeOptions().setStackMode( _type, !isStacking );

    updateStackingState();
    _onOrderChangedSignal.emit();
  }

  void updateStackingState()
  {
    bool isStacking = _city->getTradeOptions().isGoodsStacking( _type );
    std::string text;
    if( isStacking )
    {
      text = StringHelper::format( 0xff, "%s %s", _("##use_and_trade_resource##"), _("##click_here_that_stacking##") );
    }
    else
    {
      text = StringHelper::format( 0xff, "%s %s", _("##stacking_resource##"), _("##click_here_that_use_it##") );
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
  Widget::Widgets children = gbInfo->getChildren();

  foreach( child, children ) { (*child)->deleteLater(); }

  Point startDraw( 0, 5 );
  Size btnSize( gbInfo->width(), 20 );
  CityTradeOptions& copt = city->getTradeOptions();
  for( int i=Good::wheat, indexOffset=0; i < Good::goodCount; i++ )
  {
    Good::Type gtype = Good::Type( i );

    CityTradeOptions::Order tradeState = copt.getOrder( gtype );
    if( tradeState == CityTradeOptions::disabled )
    {
      continue;
    }

    int stackedQty = getStackedGoodsQty( gtype );
    bool workState = getWorkState( gtype );
    int tradeQty = copt.getExportLimit( gtype );
    
    TradeGoodInfo* btn = new TradeGoodInfo( gbInfo, Rect( startDraw + Point( 0, btnSize.height()) * indexOffset, btnSize ),
                                            gtype, stackedQty, workState, tradeState, tradeQty );
    indexOffset++;
    CONNECT( btn, onClickedA(), this, Impl::showGoodOrderManageWindow );
  }
}

bool AdvisorTradeWindow::Impl::getWorkState(Good::Type gtype )
{
  CityHelper helper( city );

  bool industryActive = false;
  FactoryList producers = helper.getProducers<Factory>( gtype );

  foreach( it, producers ) { industryActive |= (*it)->isActive(); }

  return producers.empty() ? true : industryActive;
}

int AdvisorTradeWindow::Impl::getStackedGoodsQty( Good::Type gtype )
{
  CityHelper helper( city );

  int goodsQty = 0;
  WarehouseList warehouses = helper.find< Warehouse >( building::warehouse );
  foreach( it, warehouses ) { goodsQty += (*it)->getGoodStore().qty( gtype ); }

  return goodsQty;
}

void AdvisorTradeWindow::Impl::showGoodOrderManageWindow(Good::Type type )
{
  Widget* parent = gbInfo->getParent();
  int stackedGoods = getStackedGoodsQty( type ) ;
  GoodOrderManageWindow* wnd = new GoodOrderManageWindow( parent, Rect( 50, 130, parent->width() - 45, parent->getHeight() -60 ), 
                                                          city, type, stackedGoods );

  CONNECT( wnd, onOrderChanged(), this, Impl::updateGoodsInfo );
}

void AdvisorTradeWindow::Impl::showGoodsPriceWindow()
{
  Widget* parent = gbInfo->getParent();
  Size size( 610, 180 );
  new EmpirePricesWindow( parent, -1, Rect( Point( ( parent->width() - size.width() ) / 2,
                                                   ( parent->getHeight() - size.height() ) / 2), size ), city );
}

AdvisorTradeWindow::AdvisorTradeWindow(PlayerCityPtr city, Widget* parent, int id )
: Widget( parent, id, Rect( 0, 0, 1, 1 ) ), _d( new Impl )
{
  setGeometry( Rect( Point( (parent->width() - 640 )/2, parent->getHeight() / 2 - 242 ),
               Size( 640, 432 ) ) );

  Label* title = new Label( this, Rect( 10, 10, width() - 10, 10 + 40) );
  title->setText( _("##Trade advisor##") );
  title->setFont( Font::create( FONT_3 ) );
  title->setTextAlignment( alignCenter, alignCenter );

  _d->background.reset( Picture::create( getSize() ) );
  _d->city = city;
  //main _d->_d->background
  PictureDecorator::draw( *_d->background, Rect( Point( 0, 0 ), getSize() ), PictureDecorator::whiteFrame );

  _d->btnEmpireMap = new PushButton( this, Rect( Point( 100, 398), Size( 200, 24 ) ), _("##empire_map##"), -1, false, PushButton::whiteBorderUp );
  _d->btnPrices = new PushButton( this, Rect( Point( 400, 398), Size( 200, 24 ) ), _("##show_prices##"), -1, false, PushButton::whiteBorderUp );
  _d->btnPrices->setTooltipText( _("##btn_showprice_tooltip##") );

  CONNECT( _d->btnEmpireMap, onClicked(), this, AdvisorTradeWindow::deleteLater );
  CONNECT( _d->btnPrices, onClicked(), _d.data(), Impl::showGoodsPriceWindow );

  _d->gbInfo = new GroupBox( this, Rect( 35, 55, width() - 33, getHeight() - 45 ), -1, GroupBox::blackFrame );

  _d->updateGoodsInfo();
}

void AdvisorTradeWindow::draw( GfxEngine& painter )
{
  if( !isVisible() )
    return;

  painter.drawPicture( *_d->background, screenLeft(), getScreenTop() );

  Widget::draw( painter );
}

Signal0<>& AdvisorTradeWindow::onEmpireMapRequest()
{
  return _d->btnEmpireMap->onClicked();
}

}//end namespace gui
