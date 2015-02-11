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
#include "core/utils.hpp"
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
#include "world/movableobject.hpp"
#include "world/barbarian.hpp"
#include "core/flagholder.hpp"
#include "world/playerarmy.hpp"
#include "dictionary.hpp"
#include "core/metric.hpp"

using namespace constants;
using namespace gfx;
using namespace metric;

namespace gui
{

static const char* empMapOffset = "EmpireMapWindowOffset";

class EmpireMapWindow::Impl
{
public:
  FlagHolder<int> flags;

  GameAutoPause autopause;
  Pictures border;
  Picture empireMap;
  world::CityPtr currentCity;
  Label* tooltipLabel;
  Point offset;
  bool dragging;
  Point dragStartPosition;
  PushButton* btnHelp;
  PushButton* btnExit;
  PushButton* btnTrade;
  Point lastPosition;

  Label* lbTitle;
  Widget* gbox;
  PlayerCityPtr city;

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
  void drawCell(Engine& e, Point start, int side , NColor color);
  world::ObjectPtr findObject( Point pos );
};

void EmpireMapWindow::Impl::checkCityOnMap( const Point& pos )
{
  currentCity = ptr_cast<world::City>( findObject( pos ) );

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
        world::EmpirePtr empire = city->empire();
        world::TraderoutePtr route = empire->findRoute( currentCity->name(), city->name() );
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
  world::CityList cities = city->empire()->cities();
  foreach( it, cities )
  {
    Point location = (*it)->location();
    painter.draw( (*it)->pictures(), offset + location );
#ifdef DEBUG
    drawCell( painter, offset + location - Point( 10, 10 ), 20, DefaultColors::red );
#endif
  }
}

void EmpireMapWindow::Impl::drawStatic(Engine& painter)
{
  foreach( obj, city->empire()->objects() )
  {
    if( !(*obj)->isMovable() )
    {
      painter.draw( (*obj)->pictures(), offset + (*obj)->location() );
    }
  }
}

void EmpireMapWindow::Impl::drawTradeRoutes(Engine& painter)
{
  world::TraderouteList routes = city->empire()->tradeRoutes();
  foreach( it, routes )
  {
    world::TraderoutePtr route = *it;

    const PointsArray& points = route->points();
    const Pictures& pictures = route->pictures();

#ifdef DEBUG
    for( unsigned int index=1; index < pictures.size(); index++ )
    {
      Point pos1 = offset + points[ index-1 ];
      Point pos2 = offset + points[ index ];
      painter.drawLine( DefaultColors::blue, pos1, pos2 );
      drawCell( painter, pos1 - Point( 10, 10 ), 20, DefaultColors::green );
    }
#endif

    for( unsigned int index=0; index < pictures.size(); index++ )
    {
      Point pos = offset + points[ index ];
      painter.draw( pictures[ index ], pos );
    }

    world::MerchantList merchants = route->merchants();
    foreach ( it, merchants )
    {
      painter.draw( (*it)->picture(), offset + (*it)->location() );
    }
  }
}

void EmpireMapWindow::Impl::drawMovable(Engine& painter)
{
  foreach( obj, city->empire()->objects() )
  {
    if( (*obj)->isMovable() )
    {
      world::MovableObjectPtr mobj = ptr_cast<world::MovableObject>( *obj );
      if( !mobj.isValid() )
      {
        Logger::warning( "Object %s not movable", (*obj)->name().c_str() );
        continue;
      }

      Point mappos = mobj->location();
      painter.draw( mobj->pictures(), offset + mobj->location() );

#ifdef DEBUG
      int distance = mobj->viewDistance();
      if( distance > 0 )
      {
        Point lastPos( distance * sin( 0 ), distance * cos( 0 ) );
        for( int i=1; i <= 16; i++ )
        {
          Point curPos( distance * sin( math::DEGTORAD * (360 * i / 16)),
                        distance * cos( math::DEGTORAD * (360 * i / 16)) );

          painter.drawLine( DefaultColors::blue, offset + mappos + lastPos, offset + mappos + curPos );
          lastPos = curPos;
        }
      }

      const world::Route& way = mobj->way();
      if( !way.empty() )
      {
        Point lastPos = way[ way.step ];
        for( world::Route::size_type k = way.step+1; k < way.size(); k++ )
        {
          painter.drawLine( DefaultColors::aliceBlue, offset + lastPos, offset + way[ k ] );
          lastPos = way[ k ];
        }
      }
#endif
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

void EmpireMapWindow::Impl::drawCell(Engine& e, Point start, int side, NColor color)
{
#ifdef DEBUG
  e.drawLine( color, start, start + Point( side, 0 ) );
  e.drawLine( color, start + Point( side, 0 ), start + Point( side, side ) );
  e.drawLine( color, start + Point( side, side ), start + Point( 0, side ) );
  e.drawLine( color, start + Point( 0, side ), start );
#endif
}

world::ObjectPtr EmpireMapWindow::Impl::findObject(Point pos)
{
  world::ObjectList objs = city->empire()->findObjects( -offset + pos, 15 );

  return objs.empty() ? world::ObjectPtr() : objs.front();
}

void EmpireMapWindow::Impl::createTradeRoute()
{
  if( currentCity != 0 )
  {
    world::EmpirePtr empire = city->empire();
    world::TraderoutePtr route = empire->createTradeRoute( city->name(), currentCity->name() );

    if( city.isValid() && route.isValid() && route->isSeaRoute() )
    {
      unsigned int cost = world::EmpireHelper::getTradeRouteOpenCost( empire, city->name(), currentCity->name() );
      events::GameEventPtr e = events::FundIssueEvent::create( city::Funds::sundries, -(int)cost );
      e->dispatch();

      city::Helper helper( city );
      DockList docks = helper.find<Dock>( constants::objects::dock );
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
    world::ComputerCityPtr compCity = ptr_cast<world::ComputerCity>( currentCity );
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
  world::EmpirePtr empire = city->empire();

  Point startDraw( (gbox->width() - 400) / 2, gbox->height() - 90 );
  new Label( gbox, Rect( startDraw + startInfo, Size( 70, 30 )), _("##emw_sell##") );

  const good::Store& sellgoods = currentCity->importingGoods();
  good::Product product=good::none;
  for( int k=0; product < good::goodCount; ++product )
  {
    if( sellgoods.capacity( product ) > 0  )
    {
      Label* lb = new Label( gbox, Rect( startDraw + startInfo + Point( 30 * (k+2), 0 ), Size( 24, 24 ) ) );
      lb->setBackgroundPicture( good::Helper::picture( product, true) );
      lb->setTooltipText( good::Helper::getTypeName( product ) );
      k++;
    }
  }

  Point buyPoint = startDraw + Point( 200, 0 );
  new Label( gbox, Rect( buyPoint + startInfo, Size( 70, 30 )), _("##emw_buy##") );

  const good::Store& buygoods = currentCity->exportingGoods();
  good::Product i=good::none;
  for( int k=0; i < good::goodCount; ++i )
  {
    if( buygoods.capacity( i ) > 0  )
    {
      Label* lb = new Label( gbox, Rect( buyPoint + startInfo + Point( 30 * (k+2), 0 ), Size( 24, 24 ) ) );
      lb->setBackgroundPicture(  good::Helper::picture( i, true) );
      lb->setTooltipText( good::Helper::getTypeName( i ) );
      k++;
    }
  }

  PushButton* btnOpenTrade = new PushButton( gbox, Rect( startDraw + startButton, Size( 400, 20 ) ),
                                             "", -1, false, PushButton::blackBorderUp );

  unsigned int routeOpenCost = world::EmpireHelper::getTradeRouteOpenCost( empire, city->name(), currentCity->name() );

  btnOpenTrade->setText( utils::format( 0xff, "%d %s", routeOpenCost, _("##dn_for_open_trade##")));

  CONNECT( btnOpenTrade, onClicked(), this, Impl::showOpenRouteRequestWindow );
}

void EmpireMapWindow::Impl::drawTradeRouteInfo()
{
  Point startDraw( (gbox->width() - 400) / 2, gbox->height() - 80 );
  new Label( gbox, Rect( startDraw, Size( 80, 30 )), _("##emw_sold##") );

  const good::Store& sellgoods = currentCity->importingGoods();
  good::Product i=good::none;
  for( int k=0; i < good::goodCount; ++i )
  {
    Unit maxsell = Unit::fromQty( sellgoods.capacity( i ) );
    Unit cursell = Unit::fromQty( sellgoods.qty( i ) );
    if( maxsell > 0  )
    {
      Label* lb = new Label( gbox, Rect( startDraw + Point( 80 + 100 * k, 0 ), Size( 24, 24 ) ) );
      lb->setBackgroundPicture(  good::Helper::picture( i, true) );

      std::string text = utils::format( 0xff, "%d/%d", cursell.ivalue(), maxsell.ivalue() );
      new Label( gbox, Rect( startDraw + Point( 110 + 100 * k, 0), Size( 70, 30 ) ), text );
      k++;
    }
  }

  Point buyPoint = startDraw + Point( 0, 30 );
  new Label( gbox, Rect( buyPoint, Size( 80, 30 )), _("##emw_bought##") );

  const good::Store& buygoods = currentCity->exportingGoods();
  i=good::none;
  for( int k=0; i < good::goodCount; ++i )
  {
    int maxbuy = buygoods.capacity( i ) / 100;
    int curbuy = buygoods.qty( i ) / 100;
    if( maxbuy > 0  )
    {
      Label* lb = new Label( gbox, Rect( buyPoint + Point( 80 + 100 * k, 0 ), Size( 24, 24 ) ) );
      lb->setBackgroundPicture( good::Helper::picture( i, true) );

      std::string text = utils::format( 0xff, "%d/%d", curbuy, maxbuy );
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
  DialogBox* dialog = DialogBox::confirmation( gbox->ui()->rootWidget(),
                                               _("##emp_open_trade_route##"),
                                               _("##emp_pay_open_this_route_question##") );

  CONNECT( dialog, onOk(), this, Impl::createTradeRoute );
}

EmpireMapWindow::EmpireMapWindow(Widget* parent, int id, PlayerCityPtr city )
 : Widget( parent, id, Rect( Point(0, 0), parent->size() ) ), _d( new Impl )
{
  // use some clipping to remove the right and bottom areas
  setupUI( ":/gui/empirewnd.gui" );

  _d->city = city;
  _d->tooltipLabel = 0;
  _d->autopause.activate();
  _d->empireMap = Picture::load( "the_empire", 1 );
  _d->dragging = false;
  GET_DWIDGET_FROM_UI( _d, lbTitle )

  _d->offset = game::Settings::get( empMapOffset ).toPoint();

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
  CONNECT( _d->btnHelp, onClicked(), this, EmpireMapWindow::_showHelp );

  setFlag( showCityInfo, true );
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

void EmpireMapWindow::beforeDraw(Engine& painter)
{
  Widget::beforeDraw( painter );
}

void EmpireMapWindow::setFlag(EmpireMapWindow::Flag flag, bool value)
{
  _d->flags.setFlag( flag, value );
}

bool EmpireMapWindow::onEvent( const NEvent& event )
{
  if( event.EventType == sEventMouse )
  {
    switch(event.mouse.type)
    {
    case mouseLbtnPressed:
      _d->dragStartPosition = event.mouse.pos();
      _d->dragging = true;
      bringToFront();

      if( _d->flags.isFlag( showCityInfo ) )
        _d->checkCityOnMap( _d->dragStartPosition );
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
      if ( !event.mouse.isLeftPressed() )
      {
        _d->dragging = false;
      }

      if( _d->lastPosition != event.mouse.pos() )
      {
        _d->lastPosition = event.mouse.pos();
        _changePosition();
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

void EmpireMapWindow::_changePosition()
{
  Point cursorPos = const_cast<EmpireMapWindow*>( this )->ui()->cursorPos() ;
  world::ObjectPtr obj = _d->findObject( cursorPos );

  std::string text;
  if( obj.isValid() )
  {        
    if( is_kind_of<world::ComputerCity>( obj ) )
    {
      world::ComputerCityPtr cCity = ptr_cast<world::ComputerCity>( obj );
      if( cCity->isDistantCity() )
        text = "##empmap_distant_romecity_tip##";
      else
        text = "##click_on_city_for_info##";
    }    
    else if( is_kind_of<world::City>( obj ) )
    {
      text = "##click_on_city_for_info##";
    }
    else if( is_kind_of<world::Barbarian>( obj ) )
    {
      text = "##enemy_army_threating_a_city##";
    }    
    else if( is_kind_of<world::PlayerArmy>( obj ) )
    {
      world::PlayerArmyPtr pa = ptr_cast<world::PlayerArmy>( obj );
      text = pa->mode() == world::PlayerArmy::go2home
                ? "##playerarmy_gone_to_home##"
                : "##playerarmy_gone_to_location##";
    }
  }
  else
  {
    world::EmpirePtr empire = _d->city->empire();
    world::TraderouteList routes = empire->tradeRoutes();

    foreach( it, routes )
    {
      if( (*it)->containPoint( -_d->offset + cursorPos, 4 ) )
      {
        text = (*it)->isSeaRoute() ? "##sea_route##" : "##land_route##";
      }
    }
  }

  if( _d->tooltipLabel )
  {
    _d->tooltipLabel->deleteLater();
    _d->tooltipLabel = 0;
  }

  if( !text.empty() )
  {
    Label* elm = new Label( this, Rect( 0, 0, 2, 2 ), text, true, Label::bgSimpleWhite );
    elm->setSubElement(true);
    elm->setTextAlignment( align::upperLeft, align::upperLeft );
    elm->setTextOffset( Point( 5, 5 ) );

    Size tlpSize( elm->textWidth() + 20, elm->textHeight() + 2 );
    if( tlpSize.width() > width() * 0.75 )
    {
      tlpSize.setWidth( width() * 0.5 );
      tlpSize.setHeight( elm->textHeight() * 2 + 10 );
      elm->setWordwrap( true );
    }

    Rect rect( _d->lastPosition, tlpSize );

    rect -= Point( tlpSize.width() + 20, -20 );
    elm->setGeometry( rect );

    _d->tooltipLabel = elm;
  }
}

const Point& EmpireMapWindow::_offset() const { return _d->offset; }
Widget* EmpireMapWindow::_resetInfoPanel() { _d->resetInfoPanel(); return _d->gbox; }
void EmpireMapWindow::_showHelp() {   DictionaryWindow::show( this, "empiremap" ); }

EmpireMapWindow* EmpireMapWindow::create(PlayerCityPtr city, Widget* parent, int id )
{
  EmpireMapWindow* ret = new EmpireMapWindow( parent, id, city );

  return ret;
}

EmpireMapWindow::~EmpireMapWindow()
{
  game::Settings::set( empMapOffset, _d->offset );
}

}//end namespace gui
