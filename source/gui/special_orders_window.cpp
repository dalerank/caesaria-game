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
#include "core/utils.hpp"
#include "core/logger.hpp"
#include "widget_helper.hpp"

using namespace gfx;

namespace gui
{

class VolumeButton : public PushButton
{
public:
  VolumeButton( Widget* );

  //! constructor
  VolumeButton( Widget* parent,
                const Rect& rectangle,
                int roomCap, int warehouseCap )
          : PushButton( parent, rectangle )
  {
    float prc = roomCap / (float)warehouseCap;
    if( prc < 0.50 ) { _step = 1; }
    else if( prc < 0.75 ) { _step = 2; }
    else if( prc < 1 ) { _step = 3; }
    else _step =0;

    _icon = Picture::load( "whblock", 1 );
    setBackgroundStyle( PushButton::blackBorderUp );
    setFont( Font::create( FONT_2_WHITE ) );
    _updateText();
  }

  virtual void draw( gfx::Engine& painter )
  {
    if( !visible() )
      return;

    PushButton::draw( painter );

    //for( int i=0; i < step; i++ )
    //  painter.draw( icon, absoluteRect().lefttop() + Point( 6 + i * icon.width()/2, 0)/*, &absoluteClippingRectRef() */ );
  }

  Signal1<float>& onChange() { return _onChangeSignal; }

protected:

  void _updateText()
  {
    _step = (_step+1) % 4;
    setText( _step == 0 ? "Any" : utils::format( 0xff, "%d/4", _step ) );
  }

  //! when some mouse button clicked
  virtual void _btnClicked()
  {
    PushButton::_btnClicked();

    _updateText();
    emit _onChangeSignal( _step / 4.f );
  }

  int _step;
  Picture _icon;
  Signal1<float> _onChangeSignal;
};

template< class T >
class OrderGoodWidget : public Label
{
public:
  OrderGoodWidget( Widget* parent, const Rect& rect, good::Product good, T storageBuilding )
    : Label( parent, rect, "" )
  {
    _type = good;
    _storageBuilding = storageBuilding;
    setFont( Font::create( FONT_1_WHITE ) );

    good::Store& store = _storageBuilding->store();

    _btnChangeRule = new PushButton( this, Rect( 140, 0, 140 + 240, height() ), "", -1, false, PushButton::blackBorderUp );
    _btnVolume = new VolumeButton( this, Rect( _btnChangeRule->righttop(), Size( 40, height() ) ),
                                   store.capacity( good ), store.capacity() );

    _btnChangeRule->setFont( Font::create( FONT_1_WHITE ) );
    updateBtnText();

    CONNECT( _btnChangeRule, onClicked(), this, OrderGoodWidget::changeRule );
    CONNECT( _btnVolume, onChange(), this, OrderGoodWidget::changeCapacity );
  }

  virtual void _updateTexture( gfx::Engine& painter )
  {
    Label::_updateTexture( painter );

    std::string goodName = _( "##" + good::Helper::getTypeName( _type ) + "##" );

    if( _textPictureRef() )
    {
      Font rfont = font();
      rfont.draw( *_textPictureRef(), goodName, 55, 0 );
    }
  }

  void draw(Engine &painter)
  {
    Label::draw( painter );

    Picture goodIcon = good::Helper::picture( _type );
    painter.draw( goodIcon, absoluteRect().lefttop() + Point( 15, 0 ), &absoluteClippingRectRef() );
    painter.draw( goodIcon, absoluteRect().righttop() - Point( 35, 0 ), &absoluteClippingRectRef() );
  }

  void changeCapacity( float fillingPercentage )
  {
    int storeCap = _storageBuilding->store().capacity();
    _storageBuilding->store().setCapacity( _type, storeCap * fillingPercentage );
  }

  void updateBtnText()
  {
    good::Orders::Order rule = _storageBuilding->store().getOrder( _type );
    if( rule > good::Orders::none )
    {
      Logger::warning( "OrderGoodWidget: unknown rule %d", (int)rule );
      return;
    }

    const char* ruleName[] = { "##accept##", "##reject##", "##deliver##", "##none##" };
    _btnChangeRule->setFont( Font::create( rule == good::Orders::reject ? FONT_1_RED : FONT_1_WHITE ) );
    _btnChangeRule->setText( _(ruleName[ rule ]) );
  }

  void changeRule()
  {
    good::Orders::Order rule = _storageBuilding->store().getOrder( _type );
    _storageBuilding->store().setOrder( _type, good::Orders::Order( (rule+1) % (good::Orders::none)) );
    updateBtnText();
  }

private:
  good::Product _type;
  T _storageBuilding;
  PushButton* _btnChangeRule;
  VolumeButton* _btnVolume;
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
};

template< class T >
void addOrderWidget( const int index, const good::Product good, Widget* area, T storageBuiding )
{
  Point offset( 0, 25 );
  Size wdgSize( area->width(), 25 );
  new OrderGoodWidget<T>( area, Rect( offset * index, wdgSize), good, storageBuiding );
}

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

  _d->gbOrders = new GroupBox( this, Rect( 17, 42, width() - 17, height() - 70), -1, GroupBox::blackFrame );
  _d->gbOrdersInsideArea = new Widget( _d->gbOrders, -1, Rect( 5, 5, _d->gbOrders->width() -5, _d->gbOrders->height() -5 ) );

  CONNECT( _d->btnExit, onClicked(), this, GranarySpecialOrdersWindow::deleteLater );
}


BaseSpecialOrdersWindow::~BaseSpecialOrdersWindow() {}

Widget*BaseSpecialOrdersWindow::_ordersArea()
{
  return _d->gbOrdersInsideArea;
}

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
  for( good::Product goodType=good::wheat; goodType <= good::vegetable; ++goodType )
  {
    const good::Orders::Order rule = granary->store().getOrder( goodType );
    
    if( rule != good::Orders::none )
    {
      addOrderWidget<GranaryPtr>( index, goodType, _ordersArea(), granary );
      index++;
    }
  }

  _btnToggleDevastation = new PushButton( this, Rect( 80, height() - 45, width() - 80, height() - 25 ),
                                          "", -1, false, PushButton::whiteBorderUp );

  CONNECT( _btnToggleDevastation, onClicked(), this, GranarySpecialOrdersWindow::toggleDevastation );
  _updateBtnDevastation();
}

GranarySpecialOrdersWindow::~GranarySpecialOrdersWindow() {}

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

class WarehouseSpecialOrdersWindow::Impl
{
public:
  WarehousePtr warehouse;
  PushButton* btnToggleDevastation;
  PushButton* btnTradeCenter;
};

WarehouseSpecialOrdersWindow::WarehouseSpecialOrdersWindow( Widget* parent, const Point& pos, WarehousePtr warehouse )
: BaseSpecialOrdersWindow( parent, pos, defaultHeight ), __INIT_IMPL(WarehouseSpecialOrdersWindow)
{
  __D_IMPL(d, WarehouseSpecialOrdersWindow)

  setupUI( ":/gui/warehousespecial.gui");
  setTitle( _("##warehouse_orders##") );

  d->warehouse = warehouse;
  int index=0;
  for( good::Product goodType=good::wheat; goodType <= good::marble; ++goodType )
  {
    const good::Orders::Order rule = d->warehouse->store().getOrder( goodType );

    if( rule != good::Orders::none )
    {
      addOrderWidget<WarehousePtr>( index, goodType, _ordersArea(), d->warehouse );
      index++;
    }
  }

  GET_DWIDGET_FROM_UI( d, btnToggleDevastation )
  GET_DWIDGET_FROM_UI( d, btnTradeCenter )

  CONNECT( d->btnToggleDevastation, onClicked(), this, WarehouseSpecialOrdersWindow::toggleDevastation );
  _updateBtnDevastation();
}

WarehouseSpecialOrdersWindow::~WarehouseSpecialOrdersWindow() {}

void WarehouseSpecialOrdersWindow::toggleDevastation()
{
  __D_IMPL(d, WarehouseSpecialOrdersWindow)

  d->warehouse->store().setDevastation( !d->warehouse->store().isDevastation() );
  _updateBtnDevastation();
}

void WarehouseSpecialOrdersWindow::_updateBtnDevastation()
{
  __D_IMPL(d, WarehouseSpecialOrdersWindow)

  d->btnToggleDevastation->setText( d->warehouse->store().isDevastation()
                                      ? _("##stop_warehouse_devastation##")
                                      : _("##devastate_warehouse##") );
}

}//end namespace gui
