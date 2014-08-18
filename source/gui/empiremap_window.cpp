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
// along with CaesarIA.  If not, see <http://www.gnu.org/licenses/>
//
// Copyright 2012-2014 Dalerank, dalerankn8@gmail.com

#include "empiremap_window.hpp"
#include "gfx/picturesarray.hpp"
#include "core/event.hpp"
#include "gfx/engine.hpp"
#include "texturedbutton.hpp"
#include "objects/dock.hpp"
#include "core/color.hpp"
#include "world/empire.hpp"
#include "world/computer_city.hpp"
#include "city/helper.hpp"
#include "label.hpp"
#include "core/stringhelper.hpp"
#include "core/gettext.hpp"
#include "dialogbox.hpp"
#include "good/goodstore.hpp"
#include "world/trading.hpp"
#include "city/funds.hpp"
#include "good/goodhelper.hpp"
#include "game/settings.hpp"
#include "events/showinfobox.hpp"
#include "core/logger.hpp"
#include "world/merchant.hpp"
#include "core/foreach.hpp"
#include "world/object.hpp"
#include "events/fundissue.hpp"
#include "events/showadvisorwindow.hpp"
#include "widgetescapecloser.hpp"
#include "gameautopause.hpp"
#include "gui/environment.hpp"
#include "widget_helper.hpp"

using namespace constants;
using namespace gfx;

namespace gui
{

static const char* empMapOffset = "EmpireMapWindowOffset";

class EmpireMapWindow::Impl
{
public:
  GameAutoPause autopause;
  Pictures border;
  Picture empireMap;
  world::CityPtr currentCity;
  Point offset;
  bool dragging;
  Point dragStartPosition;
  PushButton* btnHelp;
  PushButton* btnExit;
  PushButton* btnTrade;
  std::string ourCity;

  Label* lbTitle;
  Widget* gbox;
  world::EmpirePtr empire;

  void checkCityOnMap( const Point& pos );
  void showOpenRouteRequestWindow();
  void createTradeRoute();
  void drawCityGoodsInfo();
  void drawTradeRouteInfo();
  void drawCityInfo();
  void resetInfoPanel();
  void updateCityInfo();
  void drawCities( Engine& painter );
  void drawStatic( Engine& painter );
  void drawTradeRoutes( Engine& painter );
  void drawMovable( Engine& painter );
  void showTradeAdvisorWindow();
  void initBorder(Widget* p);
  world::CityPtr findCity( const Point& pos );
};

void EmpireMapWindow::Impl::checkCityOnMap( const Point& pos )
{
  currentCity = findCity( pos );

  updateCityInfo();
}

void EmpireMapWindow::Impl::updateCityInfo()
{
  resetInfoPanel();
  if( currentCity != 0 && lbTitle )
  {
    lbTitle->setText( currentCity->name() );

    if( is_kind_of<PlayerCity>( currentCity ) )
    {
      drawCityInfo();
    }
    else
    {
      world::ComputerCityPtr ccity = ptr_cast<world::ComputerCity>( currentCity );
      if( ccity.isNull() )
        return;

      if( ccity->isDistantCity() || ccity->isRomeCity() )
      {

      }
      else
      {
        world::TraderoutePtr route = empire->findRoute( currentCity->name(), ourCity );
        if( route != 0 )
        {
          drawTradeRouteInfo();
        }
        else
        {
          drawCityGoodsInfo();
        }
      }
    }
  }
  else
  {
    lbTitle->setText( "" );
  }
}

void EmpireMapWindow::Impl::drawCities(Engine& painter)
{
  world::CityList cities = empire->cities();
  foreach( it, cities )
  {
    Point location = (*it)->location();
    painter.draw( (*it)->pictures(), offset + location );
  }
}

void EmpireMapWindow::Impl::drawStatic(Engine& painter)
{
  foreach( obj, empire->objects() )
  {
    if( !(*obj)->isMovable() )
    {
      painter.draw( (*obj)->pictures(), offset + (*obj)->location() );
    }
  }
}

void EmpireMapWindow::Impl::drawTradeRoutes(Engine& painter)
{
  world::TraderouteList routes = empire->tradeRoutes();
  foreach( it, routes )
  {
    world::TraderoutePtr route = *it;

    const PointsArray& points = route->points();
    const Pictures& pictures = route->pictures();

    for( unsigned int index=0; index < pictures.size(); index++ )
    {
      painter.draw( pictures[ index ], offset + points[ index ] );
    }

    world::MerchantPtr merchant = route->merchant( 0 );
    if( merchant != 0 )
    {
      painter.draw( merchant->picture(), offset + merchant->location() );
    }
  }
}

void EmpireMapWindow::Impl::drawMovable(Engine& painter)
{
  foreach( obj, empire->objects() )
  {
    if( (*obj)->isMovable() )
    {
      painter.draw( (*obj)->pictures(), offset + (*obj)->location() );
    }
  }
}

void EmpireMapWindow::Impl::showTradeAdvisorWindow()
{
  events::GameEventPtr e = events::ShowAdvisorWindow::create( true, advisor::trading );
  e->dispatch();
}

void EmpireMapWindow::Impl::initBorder( Widget* p )
{
  Picture backgr = Picture::load( ResourceGroup::empirepnls, 4 );
  for( unsigned int y=p->height() - 120; y < p->height(); y+=backgr.height() )
  {
    for( unsigned int x=0; x < p->width(); x += backgr.width() )
    {
      border.append( backgr, Point( x, -y ) );
    }
  }

  Picture lrBorderPic = Picture::load( ResourceGroup::empirepnls, 1 );
  for( unsigned int y = 0; y < p->height(); y += lrBorderPic.height() )
  {
    border.append( lrBorderPic, Point( 0, -y ) );
    border.append( lrBorderPic, Point( p->width() - lrBorderPic.width(), -y ) );
  }

  Picture tdBorderPic = Picture::load( ResourceGroup::empirepnls, 2 );
  for( unsigned int x = 0; x < p->width(); x += tdBorderPic.width() )
  {
    border.append( tdBorderPic, Point( x, 0 ) );
    border.append( tdBorderPic, Point( x, -p->height() + tdBorderPic.height() ) );
    border.append( tdBorderPic, Point( x, -p->height() + 120 ) );
  }

  Picture corner = Picture::load( ResourceGroup::empirepnls, 3 );
  border.append( corner, Point( 0, 0 ) );    //left top
  border.append( corner, Point( 0, -p->height() + corner.height() ) ); //top right
  border.append( corner, Point( p->width() - corner.width(), 0 ) ); //left bottom
  border.append( corner, Point( p->width() - corner.width(), -p->height() + corner.height() ) ); //bottom right
  border.append( corner, Point( 0, -p->height() + 120 ) ); //left middle
  border.append( corner, Point( p->width() - corner.width(), -p->height() + 120 ) ); //right middle

  Picture leftEagle = Picture::load( ResourceGroup::empirepnls, 7 );
  Picture rightEagle = Picture::load( ResourceGroup::empirepnls, 8 );
  Picture centerPicture = Picture::load( ResourceGroup::empirepnls, 9 );
  Size eagleOffset = corner.size();

  border.append( leftEagle, Point( eagleOffset.width(), -p->height() + (120 - eagleOffset.height() + leftEagle.height() + 10) ) );
  border.append( rightEagle, Point( p->width() - eagleOffset.width() - rightEagle.width(),
                                    -p->height() + (120 - eagleOffset.height() + rightEagle.height() + 10) ) );

  border.append( centerPicture, Point( (p->width() - centerPicture.width()) / 2,
                                       -p->height() + (120 + centerPicture.height() - 20)) );
}

world::CityPtr EmpireMapWindow::Impl::findCity(const Point& pos)
{
  world::CityList cities = empire->cities();

  world::CityPtr ret;
  foreach( it, cities )
  {
    Rect rect( (*it)->location(), Size( 40 ) );
    if( rect.isPointInside( pos ) )
    {
      ret = (*it);
      break;
    }
  }

  return ret;
}

void EmpireMapWindow::Impl::createTradeRoute()
{
  if( currentCity != 0 )
  {
    unsigned int cost = world::EmpireHelper::getTradeRouteOpenCost( empire, ourCity, currentCity->name() );
    events::GameEventPtr e = events::FundIssueEvent::create( city::Funds::sundries, -(int)cost );
    e->dispatch();
    world::TraderoutePtr route = empire->createTradeRoute( ourCity, currentCity->name() );

    PlayerCityPtr plCity = ptr_cast<PlayerCity>( empire->findCity( ourCity ) );
    if( plCity.isValid() && route.isValid() && route->isSeaRoute() )
    {
      city::Helper helper( plCity );
      DockList docks = helper.find<Dock>( constants::building::dock );
      if( docks.empty() )
      {
        events::GameEventPtr e = events::ShowInfobox::create( _("##no_working_dock##" ), _( "##no_dock_for_sea_trade_routes##" ) );
        e->dispatch();
      }
    }
  }

  updateCityInfo();
}

void EmpireMapWindow::Impl::drawCityInfo()
{
  Label* lb = new Label( gbox, Rect( Point( 0, gbox->height() - 70), Size( gbox->width(), 30) ) );
  lb->setTextAlignment( align::center, align::upperLeft );
  if( is_kind_of<PlayerCity>( currentCity ) )
  {
    lb->setText( _("##empiremap_our_city##") );
  }
  else if( is_kind_of<world::ComputerCity>(currentCity) )
  {
    SmartPtr<world::ComputerCity> compCity = ptr_cast<world::ComputerCity>( currentCity );
    if( compCity->isDistantCity() )
    {
      lb->setText( _("##empiremap_distant_city##") );
    }
  }
  /*else if( currentCity->isRomeCity() )
  {
    lb->setText( _("##empiremap_rome_city##") );
  }
  */
}

void EmpireMapWindow::Impl::drawCityGoodsInfo()
{
  Point startInfo( 0, 0 );
  Point startButton( 0, 40 );


  Point startDraw( (gbox->width() - 400) / 2, gbox->height() - 90 );
  new Label( gbox, Rect( startDraw + startInfo, Size( 70, 30 )), _("##emw_sell##") );

  const GoodStore& sellgoods = currentCity->importingGoods();
  for( int i=0, k=0; i < Good::goodCount; i++ )
  {
    if( sellgoods.capacity( (Good::Type)i ) > 0  )
    {
      Label* lb = new Label( gbox, Rect( startDraw + startInfo + Point( 30 * (k+2), 0 ), Size( 24, 24 ) ) );
      lb->setBackgroundPicture( GoodHelper::picture( Good::Type(i), true) );
      lb->setTooltipText( GoodHelper::getTypeName( Good::Type(i) ) );
      k++;
    }
  }

  Point buyPoint = startDraw + Point( 200, 0 );
  new Label( gbox, Rect( buyPoint + startInfo, Size( 70, 30 )), _("##emw_buy##") );

  const GoodStore& buygoods = currentCity->exportingGoods();
  for( int i=0, k=0; i < Good::goodCount; i++ )
  {
    if( buygoods.capacity( (Good::Type)i ) > 0  )
    {
      Label* lb = new Label( gbox, Rect( buyPoint + startInfo + Point( 30 * (k+2), 0 ), Size( 24, 24 ) ) );
      lb->setBackgroundPicture(  GoodHelper::picture( Good::Type(i), true) );
      lb->setTooltipText( GoodHelper::getTypeName( Good::Type(i) ) );
      k++;
    }
  }

  PushButton* btnOpenTrade = new PushButton( gbox, Rect( startDraw + startButton, Size( 400, 20 ) ),
                                             "", -1, false, PushButton::blackBorderUp );

  unsigned int routeOpenCost = world::EmpireHelper::getTradeRouteOpenCost( empire, ourCity, currentCity->name() );

  btnOpenTrade->setText( StringHelper::format( 0xff, "%d %s", routeOpenCost, _("##dn_for_open_trade##")));

  CONNECT( btnOpenTrade, onClicked(), this, Impl::showOpenRouteRequestWindow );
}

void EmpireMapWindow::Impl::drawTradeRouteInfo()
{
  Point startDraw( (gbox->width() - 400) / 2, gbox->height() - 80 );
  new Label( gbox, Rect( startDraw, Size( 80, 30 )), _("##emw_sold##") );

  const GoodStore& sellgoods = currentCity->importingGoods();
  for( int i=0, k=0; i < Good::goodCount; i++ )
  {
    int maxsell = sellgoods.capacity( (Good::Type)i ) / 100;
    int cursell = sellgoods.qty( (Good::Type)i ) / 100;
    if( maxsell > 0  )
    {
      Label* lb = new Label( gbox, Rect( startDraw + Point( 80 + 100 * k, 0 ), Size( 24, 24 ) ) );
      lb->setBackgroundPicture(  GoodHelper::picture( Good::Type(i), true) );

      std::string text = StringHelper::format( 0xff, "%d/%d", cursell, maxsell );
      new Label( gbox, Rect( startDraw + Point( 110 + 100 * k, 0), Size( 70, 30 ) ), text );
      k++;
    }
  }

  Point buyPoint = startDraw + Point( 0, 30 );
  new Label( gbox, Rect( buyPoint, Size( 80, 30 )), _("##emw_bought##") );

  const GoodStore& buygoods = currentCity->exportingGoods();
  for( int i=0, k=0; i < Good::goodCount; i++ )
  {
    int maxbuy = buygoods.capacity( (Good::Type)i ) / 100;
    int curbuy = buygoods.qty( (Good::Type)i ) / 100;
    if( maxbuy > 0  )
    {
      Label* lb = new Label( gbox, Rect( buyPoint + Point( 80 + 100 * k, 0 ), Size( 24, 24 ) ) );
      lb->setBackgroundPicture( GoodHelper::picture( Good::Type(i), true) );

      std::string text = StringHelper::format( 0xff, "%d/%d", curbuy, maxbuy );
      new Label( gbox, Rect( buyPoint + Point( 110 + 100 * k, 0), Size( 70, 30 ) ), text );
      k++;
    }
  }
}

void EmpireMapWindow::Impl::resetInfoPanel()
{
  Widget::Widgets childs = gbox->children();
  foreach( widget, childs ) { (*widget)->deleteLater(); }
}

void EmpireMapWindow::Impl::showOpenRouteRequestWindow()
{
  DialogBox* dialog = new DialogBox( gbox->ui()->rootWidget(), Rect( 0, 0, 0, 0 ),
                                     _("##emp_open_trade_route##"), _("##emp_pay_open_this_route_question##"), 
                                     DialogBox::btnOk | DialogBox::btnCancel  );

  CONNECT( dialog, onOk(), this, Impl::createTradeRoute );
  CONNECT( dialog, onOk(), dialog, DialogBox::deleteLater );
  CONNECT( dialog, onCancel(), dialog, DialogBox::deleteLater );
}

EmpireMapWindow::EmpireMapWindow( Widget* parent, int id )
 : Widget( parent, id, Rect( Point(0, 0), parent->size() ) ), _d( new Impl )
{
  // use some clipping to remove the right and bottom areas
  setupUI( ":/gui/empirewnd.gui" );
  _d->autopause.activate();
  _d->empireMap = Picture::load( "the_empire", 1 );
  _d->dragging = false;
  GET_DWIDGET_FROM_UI( _d, lbTitle )

  _d->offset = GameSettings::get( empMapOffset ).toPoint();

  WidgetEscapeCloser::insertTo( this );

  _d->initBorder( this );

  GET_DWIDGET_FROM_UI( _d, gbox )
  if( _d->gbox ) _d->gbox->sendToBack();

  GET_DWIDGET_FROM_UI( _d, btnHelp )
  GET_DWIDGET_FROM_UI( _d, btnExit )
  GET_DWIDGET_FROM_UI( _d, btnTrade )

  CONNECT( _d->btnExit, onClicked(), this, EmpireMapWindow::deleteLater );
  CONNECT( _d->btnTrade, onClicked(), this, EmpireMapWindow::deleteLater );
  CONNECT( _d->btnTrade, onClicked(), _d.data(), Impl::showTradeAdvisorWindow );
}

void EmpireMapWindow::draw(gfx::Engine& engine )
{
  if( !visible() )
    return;

  engine.draw( _d->empireMap, _d->offset );  

  //draw static objects
  _d->drawStatic( engine );

  _d->drawTradeRoutes( engine );

  _d->drawCities( engine );

  //draw movable objects
  _d->drawMovable( engine );

  engine.draw( _d->border, absoluteRect().lefttop(), &absoluteClippingRectRef() );

  Widget::draw( engine );
}

bool EmpireMapWindow::onEvent( const NEvent& event )
{
  if( event.EventType == sEventMouse )
  {
    switch(event.mouse.type)
    {
    case mouseLbtnPressed:
      _d->dragStartPosition = event.mouse.pos();
      _d->dragging = true;//_d->flags.isFlag( draggable );
      bringToFront();

      _d->checkCityOnMap( _d->dragStartPosition - _d->offset );
    break;

    case mouseRbtnRelease:
      deleteLater();
      _d->dragging = false;
    break;

    case mouseLbtnRelease:
      _d->dragging = false;
    break;

    case mouseMoved:
      {
        //bool t = _d->dragging;

        if ( !event.mouse.isLeftPressed() )
        {
          _d->dragging = false;
        }

        if( _d->dragging )
        {
          // gui window should not be dragged outside its parent
          if( _d->offset.x() > 0
              || _d->offset.x() + _d->empireMap.width() < (int)width()
              || _d->offset.y() > 0
              || _d->offset.y() + _d->empireMap.height() < (int)height()-120 )
          {
            break;
          }

          _d->offset += (event.mouse.pos() - _d->dragStartPosition);
          _d->dragStartPosition = event.mouse.pos();

          _d->offset.setX( math::clamp<int>( _d->offset.x(), -_d->empireMap.width() + width(), 0 ) );
          _d->offset.setY( math::clamp<int>( _d->offset.y(), -_d->empireMap.height() + height() - 120, 0 ) );
        }
      }
    break;
    
    default:
    break;
    }

    return true;
  }

  return Widget::onEvent( event );
}

std::string EmpireMapWindow::tooltipText() const
{
  world::CityPtr wCity = _d->findCity( const_cast<EmpireMapWindow*>( this )->ui()->cursorPos() );

  if( wCity.isValid() )
  {
    return "##click_on_city_for_info##";
  }

  return Widget::tooltipText();
}

EmpireMapWindow* EmpireMapWindow::create(world::EmpirePtr empire, PlayerCityPtr city, Widget* parent, int id )
{
  EmpireMapWindow* ret = new EmpireMapWindow( parent, id );
  ret->_d->empire = empire;
  ret->_d->ourCity = city->name();

  return ret;
}

EmpireMapWindow::~EmpireMapWindow()
{
  GameSettings::set( empMapOffset, _d->offset );
}

}//end namespace gui
