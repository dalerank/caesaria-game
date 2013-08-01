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

#include "oc3_advisor_trade_window.hpp"
#include "oc3_picture.hpp"
#include "oc3_picture_decorator.hpp"
#include "oc3_gettext.hpp"
#include "oc3_goodhelper.hpp"
#include "oc3_pushbutton.hpp"
#include "oc3_gui_label.hpp"
#include "oc3_resourcegroup.hpp"
#include "oc3_stringhelper.hpp"
#include "oc3_gfx_engine.hpp"
#include "oc3_gettext.hpp"
#include "oc3_groupbox.hpp"
#include "oc3_factory_building.hpp"
#include "oc3_city.hpp"
#include "oc3_warehouse.hpp"
#include "oc3_goodstore.hpp"
#include "oc3_texturedbutton.hpp"

class TradeGoodInfo : public PushButton
{
public:
  typedef enum { importGoods=0, useGoods, stackGoods, exportGoods } TradeState;
  TradeGoodInfo( Widget* parent, const Rect& rect, GoodType good, int qty, bool enable, TradeState trade, int tradeQty )
    : PushButton( parent, rect, "" )
  {
    _type = good;
    _qty = qty;
    _enable = enable;
    _trade = trade;
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
    background->draw( _goodPicture, getWidth() - 20 - _goodPicture.getWidth(), 0, false );

    if( _getTextPicture( state ) != 0 )
    {
      Font font = getFont( state );    
      PictureRef& textPic = _getTextPicture( state );
      font.draw( *textPic, _goodName.c_str(), 55, 0 );   
      font.draw( *textPic, StringHelper::format( 0xff, "%d", _qty), 190, 0 );
      font.draw( *textPic, _enable ? "" : _("##disable##"), 260, 0 );

      std::string ruleName[] = { _("##import##"), "", _("##stacking##"), _("##export##") };
      std::string tradeStateText = ruleName[ _trade ];
      switch( _trade )
      {
      case useGoods:
      case stackGoods:
      case importGoods: 
        tradeStateText = ruleName[ _trade ];
      break;
      
      case exportGoods:
        tradeStateText = StringHelper::format( 0xff, "%s %d", ruleName[ _trade ].c_str(), _tradeQty );
      break;
      }
      font.draw( *textPic, tradeStateText, 340, 0 );

      if( state == stHovered ) 
      {
        PictureDecorator::draw( *background, Rect( 50, 0, getWidth() - 50, getHeight() ), PictureDecorator::brownBorder, false );
      }
    }
  }

  Signal1<GoodType>& onClickedA() { return _onClickedASignal; }

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
  TradeState _trade;
  int _tradeQty;
  GoodType _type;
  std::string _goodName;
  Picture _goodPicture;

oc3_signals private:
  Signal1<GoodType> _onClickedASignal;
};

class AdvisorTradeWindow::Impl
{
public:
  PictureRef background;
  PushButton* btnEmpireMap;
  PushButton* btnPrices; 
  GroupBox* gbInfo;
  CityPtr city;

  TradeGoodInfo::TradeState getTradeState( GoodType gtype );

  bool getWorkState( GoodType gtype );
  int  getStackedGoodsQty( GoodType gtype );
  int  getTradeQty( GoodType gtype );
  void updateGoodsInfo();
  void showGoodOrderManageWindow( GoodType type );
};

class GoodOrderManageWindow : public Widget
{
public:
  GoodOrderManageWindow( Widget* parent, const Rect& rectangle, CityPtr city, GoodType type, int stackedGoods )
    : Widget( parent, -1, rectangle )
  {
    _background.reset( Picture::create( getSize() ) );
    PictureDecorator::draw( *_background, Rect( Point( 0, 0 ), getSize() ), PictureDecorator::whiteFrame );

    const Picture& iconGood = GoodHelper::getPicture( type );
    _background->draw( iconGood, Point( 10, 10 ) );

    Label* lbTitle = new Label( this, Rect( 40, 10, getWidth() - 10, 10 + 30), GoodHelper::getName( type ) );
    lbTitle->setFont( Font::create( FONT_3 ) );

    CityHelper helper( city );
    int workFactoryCount=0, idleFactoryCount=0;
    std::list< FactoryPtr > factories = helper.getProducers<Factory>( type );
    for( std::list< FactoryPtr >::const_iterator it=factories.begin(); it != factories.end(); it++ )
    {
      ((*it)->standIdle() ? idleFactoryCount : workFactoryCount ) += 1;
    }

    std::string text = StringHelper::format( 0xff, "%d %s, %d %s", workFactoryCount, _("##work##"), idleFactoryCount, _("##idle_factory_in_city##") );
    Label* lbIndustry = new Label( this, Rect( 40, 40, getWidth() - 10, 40 + 20 ), text );

    text = StringHelper::format( 0xff, "%d %s", stackedGoods, _("##qty_stacked_in_city_warehouse##") );
    Label* lbStacked = new Label( this, Rect( 40, 60, getWidth() - 10, 60 + 20 ), text );

    _btnTradeState = new PushButton( this, Rect( 50, 85, getWidth() - 60, 85 + 30), "trade_state", -1, false, PushButton::WhiteBorderUp );
    PushButton* btnIndustryEnable = new PushButton( this, Rect( 50, 125, getWidth() - 60, 125 + 30), "industry_state", -1, false, PushButton::WhiteBorderUp );
    PushButton* btnStackingState = new PushButton( this, Rect( 50, 160, getWidth() - 60, 160 + 50), "stacking_state", -1, false, PushButton::WhiteBorderUp );

    TexturedButton* btnExit = new TexturedButton( this, Point( getWidth() - 34, getHeight() - 34 ), Size( 24 ), -1, ResourceMenu::exitInfBtnPicId );
    TexturedButton* btnHelp = new TexturedButton( this, Point( 11, getHeight() - 34 ), Size( 24 ), -1, ResourceMenu::helpInfBtnPicId );

    CONNECT( btnExit, onClicked(), this, GoodOrderManageWindow::deleteLater );
    CONNECT( _btnTradeState, onClicked(), this, GoodOrderManageWindow::changeTradeState );
  }

  void draw( GfxEngine& painter )
  {
    if( !isVisible() )
      return;

    painter.drawPicture( *_background, getScreenLeft(), getScreenTop() );

    Widget::draw( painter );
  }

  void changeTradeState()
  {
    CityTradeOptions& ctrading = _city->getTradeOptions();
  }

oc3_signals public:
  Signal0<>& onOrderChanged() { return _onOrderChangedSignal; }

private:
  CityPtr _city;
  GoodType _type;
  PictureRef _background;
  PushButton* _btnTradeState;

oc3_signals private:
  Signal0<> _onOrderChangedSignal;
};

void AdvisorTradeWindow::Impl::updateGoodsInfo()
{
  Widget::Widgets children = gbInfo->getChildren();
  for( Widget::ChildIterator it=children.begin(); it != children.end(); it++ )
  {
    (*it)->deleteLater();
  }

  Point startDraw( 0, 5 );
  Size btnSize( gbInfo->getWidth(), 20 );
  for( int i=G_WHEAT, indexOffset=0; i < G_MAX; i++, indexOffset++ )
  {
    GoodType gtype = GoodType( i );

    int stackedQty = getStackedGoodsQty( gtype );
    bool workState = getWorkState( gtype );
    TradeGoodInfo::TradeState tradeState = getTradeState( gtype );
    int tradeQty = getTradeQty( gtype );

    TradeGoodInfo* btn = new TradeGoodInfo( gbInfo, Rect( startDraw + Point( 0, btnSize.getHeight()) * indexOffset, btnSize ),
                                            gtype, stackedQty, workState, tradeState, tradeQty );

    CONNECT( btn, onClickedA(), this, Impl::showGoodOrderManageWindow );
  }
}

bool AdvisorTradeWindow::Impl::getWorkState( GoodType gtype )
{
  CityHelper helper( city );

  bool industryActive = false;
  const std::list< FactoryPtr > producers = helper.getProducers< Factory >( gtype );
  for( std::list< FactoryPtr >::const_iterator it=producers.begin(); it != producers.end(); it++ )
  {
    industryActive |= (*it)->isActive();
  }

  return producers.empty() ? true : industryActive;
}

int AdvisorTradeWindow::Impl::getStackedGoodsQty( GoodType gtype )
{
  CityHelper helper( city );

  int goodsQty = 0;
  const std::list< WarehousePtr > warehouses = helper.getBuildings< Warehouse >( B_WAREHOUSE );
  for( std::list< WarehousePtr >::const_iterator it=warehouses.begin(); it != warehouses.end(); it++ )
  {
    goodsQty += (*it)->getGoodStore().getCurrentQty( gtype );
  }

  return goodsQty;
}

TradeGoodInfo::TradeState AdvisorTradeWindow::Impl::getTradeState( GoodType gtype )
{
  return TradeGoodInfo::useGoods;
}

int AdvisorTradeWindow::Impl::getTradeQty( GoodType gtype )
{
  return 0;
}

void AdvisorTradeWindow::Impl::showGoodOrderManageWindow( GoodType type )
{
  Widget* parent = gbInfo->getParent();
  int stackedGoods = getStackedGoodsQty( type ) ;
  GoodOrderManageWindow* wnd = new GoodOrderManageWindow( parent, Rect( 50, 130, parent->getWidth() - 45, parent->getHeight() -60 ), 
                                                          city, type, stackedGoods );

  CONNECT( wnd, onOrderChanged(), this, Impl::updateGoodsInfo );
}

AdvisorTradeWindow::AdvisorTradeWindow( CityPtr city, Widget* parent, int id ) 
: Widget( parent, id, Rect( 0, 0, 1, 1 ) ), _d( new Impl )
{
  setGeometry( Rect( Point( (parent->getWidth() - 640 )/2, parent->getHeight() / 2 - 242 ),
               Size( 640, 432 ) ) );

  Label* title = new Label( this, Rect( 10, 10, getWidth() - 10, 10 + 40) );
  title->setText( _("##Trade advisor##") );
  title->setFont( Font::create( FONT_3 ) );
  title->setTextAlignment( alignCenter, alignCenter );

  _d->background.reset( Picture::create( getSize() ) );
  _d->city = city;
  //main _d->_d->background
  PictureDecorator::draw( *_d->background, Rect( Point( 0, 0 ), getSize() ), PictureDecorator::whiteFrame );

  _d->btnEmpireMap = new PushButton( this, Rect( Point( 100, 398), Size( 200, 24 ) ), _("##empire_map##"), -1, false, PushButton::WhiteBorderUp );
  _d->btnPrices = new PushButton( this, Rect( Point( 400, 398), Size( 200, 24 ) ), _("##show_prices##"), -1, false, PushButton::WhiteBorderUp );

  CONNECT( _d->btnEmpireMap, onClicked(), this, AdvisorTradeWindow::deleteLater );

  _d->gbInfo = new GroupBox( this, Rect( 35, 55, getWidth() - 33, getHeight() - 45 ), -1, GroupBox::blackFrame );

  _d->updateGoodsInfo();
}

void AdvisorTradeWindow::draw( GfxEngine& painter )
{
  if( !isVisible() )
    return;

  painter.drawPicture( *_d->background, getScreenLeft(), getScreenTop() );

  Widget::draw( painter );
}

Signal0<>& AdvisorTradeWindow::onEmpireMapRequest()
{
  return _d->btnEmpireMap->onClicked();
}
