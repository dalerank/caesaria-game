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

#include <cstdio>

#include "special_orders_window.hpp"
#include "gfx/engine.hpp"
#include "groupbox.hpp"
#include "label.hpp"
#include "texturedbutton.hpp"
#include "core/gettext.hpp"
#include "gfx/decorator.hpp"
#include "core/event.hpp"
#include "objects/granary.hpp"
#include "objects/warehouse.hpp"
#include "good/goodhelper.hpp"
#include "good/goodstore.hpp"
#include "good/goodorders.hpp"
#include "core/stringhelper.hpp"
#include "core/logger.hpp"

using namespace gfx;

namespace gui
{

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

    _btnChangeRule = new PushButton( this, Rect( 140, 0, 140 + 240, height() ), "", -1, false, PushButton::blackBorderUp );
    _btnChangeRule->setFont( Font::create( FONT_1_WHITE ) );
    updateBtnText();

    CONNECT( _btnChangeRule, onClicked(), this, OrderGoodWidget::changeGranaryRule );
  }

  virtual void _updateTexture( gfx::Engine& painter )
  {
    Label::_updateTexture( painter );

    Picture goodIcon = GoodHelper::getPicture( _type );
    std::string goodName = _( "##" + GoodHelper::getTypeName( _type ) + "##" );

    if( _textPictureRef() )
    {
      _textPictureRef()->draw( goodIcon, 15, 0, false );
      _textPictureRef()->draw( goodIcon, 390, 0, false );

      Font rfont = font();
      rfont.draw( *_textPictureRef(), goodName, 55, 0 );
    }
  }

  void updateBtnText()
  {
    GoodOrders::Order rule = _storageBuilding->store().getOrder( _type );
    const char* ruleName[] = { "##accept##", "##reject##", "##deliver##", "##none##" };
    _btnChangeRule->setFont( Font::create( rule == GoodOrders::reject ? FONT_1_RED : FONT_1_WHITE ) );
    _btnChangeRule->setText( _(ruleName[ rule ]) );
  }

  void changeGranaryRule()
  {
    GoodOrders::Order rule = _storageBuilding->store().getOrder( _type );
    _storageBuilding->store().setOrder( _type, GoodOrders::Order( (rule+1) % (GoodOrders::none)) );
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
  Pictures bgPicture;
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
    Size wdgSize( gbOrdersInsideArea->width(), 25 );
    new OrderGoodWidget<T>( gbOrdersInsideArea, Rect( offset * index, wdgSize), good, storageBuiding );
  }
};

BaseSpecialOrdersWindow::BaseSpecialOrdersWindow( Widget* parent, const Point& pos, int h )
  : Window( parent, Rect( pos, Size( 510, h ) ), "" ), _d( new Impl )
{
  // create the title
  _d->lbTitle = new Label( this, Rect( 50, 10, width()-50, 10 + 30 ), "", true );
  _d->lbTitle->setFont( Font::create( FONT_5 ) );
  _d->lbTitle->setTextAlignment( align::center, align::center );

  _d->btnExit = new TexturedButton( this, Point( 472, height() - 39 ), Size( 24 ), -1, ResourceMenu::exitInfBtnPicId );
  _d->btnExit->setTooltipText( _("##infobox_tooltip_exit##") );

  _d->btnHelp = new TexturedButton( this, Point( 14, height() - 39 ), Size( 24 ), -1, ResourceMenu::helpInfBtnPicId );
  _d->btnHelp->setTooltipText( _("##infobox_tooltip_help##") );

  CONNECT( _d->btnExit, onClicked(), this, GranarySpecialOrdersWindow::deleteLater );

  _d->gbOrders = new GroupBox( this, Rect( 17, 42, width() - 17, height() - 70), -1, GroupBox::blackFrame );
  _d->gbOrdersInsideArea = new Widget( _d->gbOrders, -1, Rect( 5, 5, _d->gbOrders->width() -5, _d->gbOrders->height() -5 ) );
}


BaseSpecialOrdersWindow::~BaseSpecialOrdersWindow() {}

void BaseSpecialOrdersWindow::draw(gfx::Engine& engine )
{
  Window::draw( engine );
}

bool BaseSpecialOrdersWindow::isPointInside( const Point& point ) const
{
  //resolve all screen for self using
  return parent()->absoluteRect().isPointInside( point );
}

bool BaseSpecialOrdersWindow::onEvent( const NEvent& event)
{
  switch( event.EventType )
  {
  case sEventMouse:
    if( event.mouse.type == mouseRbtnRelease )
    {
      deleteLater();
      return true;
    }
    else if( event.mouse.type == mouseLbtnRelease )
    {
      return true;
    }
    break;

  default:
  break;
  }

  return Widget::onEvent( event );
}

void BaseSpecialOrdersWindow::setTitle( const std::string& text ){  _d->lbTitle->setText( text );}

GranarySpecialOrdersWindow::GranarySpecialOrdersWindow( Widget* parent, const Point& pos, GranaryPtr granary )
: BaseSpecialOrdersWindow( parent, pos, defaultHeight )
{
  setTitle( _("##granary_orders##") );
  int index=0;
  _granary = granary;
  for( int goodType=Good::wheat; goodType <= Good::vegetable; goodType++ )
  {
    const GoodOrders::Order rule = granary->store().getOrder( (Good::Type)goodType );
    
    if( rule != GoodOrders::none )
    {
      _d->addOrderWidget<GranaryPtr>( index, (Good::Type)goodType, granary );
      index++;
    }
  }

  _btnToggleDevastation = new PushButton( this, Rect( 80, height() - 45, width() - 80, height() - 25 ),
                                          "", -1, false, PushButton::whiteBorderUp );

  CONNECT( _btnToggleDevastation, onClicked(), this, GranarySpecialOrdersWindow::toggleDevastation );
  _updateBtnDevastation();
}

void GranarySpecialOrdersWindow::toggleDevastation()
{
  _granary->store().setDevastation( !_granary->store().isDevastation() );
  _updateBtnDevastation();
}

void GranarySpecialOrdersWindow::_updateBtnDevastation()
{
  _btnToggleDevastation->setText( _granary->store().isDevastation() 
                                    ? _("##stop_granary_devastation##")
                                    : _("##devastate_granary##") );
}

WarehouseSpecialOrdersWindow::WarehouseSpecialOrdersWindow( Widget* parent, const Point& pos, WarehousePtr warehouse )
: BaseSpecialOrdersWindow( parent, pos, defaultHeight )
{
  setTitle( _("##warehouse_orders##") );

  _warehouse = warehouse;
  int index=0;
  for( int goodType=Good::wheat; goodType <= Good::marble; goodType++ )
  {
    const GoodOrders::Order rule = _warehouse->store().getOrder( (Good::Type)goodType );

    if( rule != GoodOrders::none )
    {
      _d->addOrderWidget<WarehousePtr>( index, (Good::Type)goodType, _warehouse );
      index++;
    }
  }

  _btnToggleDevastation = new PushButton( this, Rect( 80, height() - 45, width() - 80, height() - 25 ),
                                          "", -1, false, PushButton::whiteBorderUp );

  _btnTradeCenter = new PushButton( this, Rect( 80, height() - 70, width() - 80, height() - 50 ),
                                   _("##trade_center##"), -1, false, PushButton::whiteBorderUp );

  CONNECT( _btnToggleDevastation, onClicked(), this, WarehouseSpecialOrdersWindow::toggleDevastation );
  _updateBtnDevastation();
}

void WarehouseSpecialOrdersWindow::toggleDevastation()
{
  _warehouse->store().setDevastation( !_warehouse->store().isDevastation() );
  _updateBtnDevastation();
}

void WarehouseSpecialOrdersWindow::_updateBtnDevastation()
{
  _btnToggleDevastation->setText( _warehouse->store().isDevastation() 
                                      ? _("##stop_warehouse_devastation##")
                                      : _("##devastate_warehouse##") );
}

}//end namespace gui
