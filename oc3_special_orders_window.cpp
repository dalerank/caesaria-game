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

#include <cstdio>

#include "oc3_special_orders_window.hpp"
#include "oc3_gfx_engine.hpp"
#include "oc3_groupbox.hpp"
#include "oc3_gui_label.hpp"
#include "oc3_texturedbutton.hpp"
#include "oc3_gettext.hpp"
#include "oc3_picture_decorator.hpp"
#include "oc3_event.hpp"
#include "oc3_granary.hpp"
#include "oc3_building_warehouse.hpp"
#include "oc3_goodhelper.hpp"
#include "oc3_goodstore.hpp"
#include "oc3_goodorders.hpp"

template< class T >
class OrderGoodWidget : public Label
{
public:
  OrderGoodWidget( Widget* parent, const Rect& rect, Good::Type good, T storageBuilding )
    : Label( parent, rect, "" )
  {
    _type = good;
    _storageBuilding = storageBuilding;
    setFont( Font::create( FONT_1_WHITE ) );

    _btnChangeRule = new PushButton( this, Rect( 140, 0, 140 + 240, getHeight() ), "", -1, false, PushButton::blackBorderUp );
    _btnChangeRule->setFont( Font::create( FONT_1_WHITE ) );
    updateBtnText();

    CONNECT( _btnChangeRule, onClicked(), this, OrderGoodWidget::changeGranaryRule );
  }

  virtual void _updateTexture( GfxEngine& painter )
  {
    Label::_updateTexture( painter );

    Picture goodIcon = GoodHelper::getPicture( _type );
    std::string goodName = GoodHelper::getName( _type );

    if( getTextPicture() )
    {
      getTextPicture()->draw( goodIcon, 15, 0, false );
      getTextPicture()->draw( goodIcon, 390, 0, false );

      Font font = getFont();    
      font.draw( *getTextPicture(), goodName, 55, 0 );   
    }
  }

  void updateBtnText()
  {
    GoodOrders::Order rule = _storageBuilding->getGoodStore().getOrder( _type );
    std::string ruleName[] = { _("##accept##"), _("##reject##"), _("##deliver##"), _("##none##") };
    _btnChangeRule->setFont( Font::create( rule == GoodOrders::reject ? FONT_1_RED : FONT_1_WHITE ) );
    _btnChangeRule->setText( ruleName[ rule ] );
  }

  void changeGranaryRule()
  {
    GoodOrders::Order rule = _storageBuilding->getGoodStore().getOrder( _type );
    _storageBuilding->getGoodStore().setOrder( _type, GoodOrders::Order( (rule+1) % (GoodOrders::none)) );
    updateBtnText();
  }

private:
  Good::Type _type;
  T _storageBuilding;
  PushButton* _btnChangeRule;
};

class BaseSpecialOrdersWindow::Impl
{
public:
  PictureRef bgPicture;
  GroupBox* gbOrders;
  Widget* gbOrdersInsideArea;
  Label* lbTitle;
  PushButton* btnExit;
  PushButton* btnHelp;
  PushButton* btnEmpty;

  template< class T >
  void addOrderWidget( const int index, const Good::Type good, T storageBuiding )
  {
    Point offset( 0, 25 );
    Size wdgSize( gbOrdersInsideArea->getWidth(), 25 );
    new OrderGoodWidget<T>( gbOrdersInsideArea, Rect( offset * index, wdgSize), good, storageBuiding );
  }
};

BaseSpecialOrdersWindow::BaseSpecialOrdersWindow( Widget* parent, const Point& pos )
  : Widget( parent, -1, Rect( pos, Size( 510, 450 ) ) ), _d( new Impl )
{
  // create the title
  _d->lbTitle = new Label( this, Rect( 50, 10, getWidth()-50, 10 + 30 ), "", true );
  _d->lbTitle->setFont( Font::create( FONT_3 ) );
  _d->lbTitle->setTextAlignment( alignCenter, alignCenter );

  _d->btnExit = new TexturedButton( this, Point( 472, getHeight() - 39 ), Size( 24 ), -1, ResourceMenu::exitInfBtnPicId );
  _d->btnExit->setTooltipText( _("##infobox_tooltip_exit##") );

  _d->btnHelp = new TexturedButton( this, Point( 14, getHeight() - 39 ), Size( 24 ), -1, ResourceMenu::helpInfBtnPicId );
  _d->btnHelp->setTooltipText( _("##infobox_tooltip_help##") );

  CONNECT( _d->btnExit, onClicked(), this, GranarySpecialOrdersWindow::deleteLater );

  _d->bgPicture.reset( Picture::create( getSize() ) );

  // draws the box and the inner black box
  PictureDecorator::draw( *_d->bgPicture, Rect( Point( 0, 0 ), getSize() ), PictureDecorator::whiteFrame );

  _d->gbOrders = new GroupBox( this, Rect( 17, 42, getWidth() - 17, getHeight() - 70), -1, GroupBox::blackFrame );  
  _d->gbOrdersInsideArea = new Widget( _d->gbOrders, -1, Rect( 5, 5, _d->gbOrders->getWidth() -5, _d->gbOrders->getHeight() -5 ) );
}


BaseSpecialOrdersWindow::~BaseSpecialOrdersWindow()
{
  
}

void BaseSpecialOrdersWindow::draw( GfxEngine& engine )
{
  engine.drawPicture( *_d->bgPicture, getScreenLeft(), getScreenTop() );
  Widget::draw( engine );
}

bool BaseSpecialOrdersWindow::isPointInside( const Point& point ) const
{
  //resolve all screen for self using
  return getParent()->getAbsoluteRect().isPointInside( point );
}

bool BaseSpecialOrdersWindow::onEvent( const NEvent& event)
{
  switch( event.EventType )
  {
  case OC3_MOUSE_EVENT:
    if( event.MouseEvent.Event == OC3_RMOUSE_LEFT_UP )
    {
      deleteLater();
      return true;
    }
    else if( event.MouseEvent.Event == OC3_LMOUSE_LEFT_UP )
    {
      return true;
    }
    break;

  default:
  break;
  }

  return Widget::onEvent( event );
}

void BaseSpecialOrdersWindow::setTitle( const std::string& text )
{
  _d->lbTitle->setText( text );
}

GranarySpecialOrdersWindow::GranarySpecialOrdersWindow( Widget* parent, const Point& pos, GranaryPtr granary )
: BaseSpecialOrdersWindow( parent, pos )
{
  setTitle( _("##granary_orders##") );
  int index=0;
  _granary = granary;
  for( int goodType=Good::wheat; goodType <= Good::vegetable; goodType++ )
  {
    const GoodOrders::Order rule = granary->getGoodStore().getOrder( (Good::Type)goodType );
    
    if( rule != GoodOrders::none )
    {
      _d->addOrderWidget<GranaryPtr>( index, (Good::Type)goodType, granary );
      index++;
    }
  }

  _btnToggleDevastation = new PushButton( this, Rect( 80, getHeight() - 45, getWidth() - 80, getHeight() - 25 ),
                                          "", -1, false, PushButton::whiteBorderUp );

  CONNECT( _btnToggleDevastation, onClicked(), this, GranarySpecialOrdersWindow::toggleDevastation );
  _updateBtnDevastation();
}

void GranarySpecialOrdersWindow::toggleDevastation()
{
  _granary->getGoodStore().setDevastation( !_granary->getGoodStore().isDevastation() );
  _updateBtnDevastation();
}

void GranarySpecialOrdersWindow::_updateBtnDevastation()
{
  _btnToggleDevastation->setText( _granary->getGoodStore().isDevastation() 
                                    ? _("##stop_granary_devastation##")
                                    : _("##devastate_granary##") );
}

WarehouseSpecialOrdersWindow::WarehouseSpecialOrdersWindow( Widget* parent, const Point& pos, WarehousePtr warehouse )
: BaseSpecialOrdersWindow( parent, pos )
{
  setTitle( _("##warehouse_orders##") );

  _warehouse = warehouse;
  int index=0;
  for( int goodType=Good::wheat; goodType <= Good::marble; goodType++ )
  {
    const GoodOrders::Order rule = _warehouse->getGoodStore().getOrder( (Good::Type)goodType );

    if( rule != GoodOrders::none )
    {
      _d->addOrderWidget<WarehousePtr>( index, (Good::Type)goodType, _warehouse );
      index++;
    }
  }

  _btnToggleDevastation = new PushButton( this, Rect( 80, getHeight() - 45, getWidth() - 80, getHeight() - 25 ),
                                          "", -1, false, PushButton::whiteBorderUp );

  _btnTradeCenter = new PushButton( this, Rect( 80, getHeight() - 70, getWidth() - 80, getHeight() - 50 ),
                                   _("##Trace center##"), -1, false, PushButton::whiteBorderUp );

  CONNECT( _btnToggleDevastation, onClicked(), this, WarehouseSpecialOrdersWindow::toggleDevastation );
  _updateBtnDevastation();
}

void WarehouseSpecialOrdersWindow::toggleDevastation()
{
  _warehouse->getGoodStore().setDevastation( !_warehouse->getGoodStore().isDevastation() );
  _updateBtnDevastation();
}

void WarehouseSpecialOrdersWindow::_updateBtnDevastation()
{
  _btnToggleDevastation->setText( _warehouse->getGoodStore().isDevastation() 
                                      ? _("##stop_warehouse_devastation##")
                                      : _("##devastate_warehouse##") );
}
