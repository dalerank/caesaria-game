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
// Copyright 2012-2015 Dalerank, dalerankn8@gmail.com

#include "empiremap_window.hpp"
#include "gfx/picturesarray.hpp"
#include "core/event.hpp"
#include "gfx/engine.hpp"
#include "texturedbutton.hpp"
#include "objects/dock.hpp"
#include "core/color.hpp"
#include "world/empire.hpp"
#include "world/computer_city.hpp"
#include "city/statistic.hpp"
#include "gfx/decorator.hpp"
#include "label.hpp"
#include "core/utils.hpp"
#include "core/gettext.hpp"
#include "dialogbox.hpp"
#include "good/store.hpp"
#include "world/trading.hpp"
#include "game/funds.hpp"
#include "good/helper.hpp"
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
#include "game/resourcegroup.hpp"
#include "city/states.hpp"

using namespace gfx;
using namespace metric;
using namespace events;
using namespace gui::dialog;

namespace gui
{

static const char* empMapOffset = "EmpireMapWindowOffset";
struct Dragging
{
  bool active;
  Point start;
  Point last;
};

struct Line
{
  Point p1, p2;
  NColor color;
};

class Lines : public std::vector<Line>
{
public:
  void add( const NColor& color, const Point& p1, const Point& p2 )
  {
    Line a = { p1, p2, color };
    push_back( a );
  }
};

class EmpireMapWindow::Impl
{
public:
  FlagHolder<int> flags;

  struct {
    Batch batch;
    Pictures nobatch;
  } border;

  GameAutoPause autopause;
  Dragging drag;
  Picture empireMap;
  world::CityPtr currentCity;
  Point offset;
  Label* tooltipLabel;
  PushButton* btnHelp;
  PushButton* btnExit;
  PushButton* btnAi;
  PushButton* btnTrade;
  Label* lbTitle;
  Lines lines;
  Widget* gbox;
  PlayerCityPtr city;
  unsigned int bottonMargin;

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
  void drawLines( Engine& painter );
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

      if( ccity->isDistantCity() || ccity->states().romeCity )
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
  Point location;
  Picture pic;
  foreach( it, cities )
  {
    location = (*it)->location();
    pic = (*it)->picture();
    painter.draw( (*it)->pictures(), offset + location - Point( pic.width() / 2, pic.height() / 2 ) );
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
      lines.add( DefaultColors::blue, pos1, pos2 );
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
      world::MovableObjectPtr mobj = obj->as<world::MovableObject>();
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
          Point curPos( distance * sin( math::DEGTORAD * (math::DEGREE360 * i / 16)),
                        distance * cos( math::DEGTORAD * (math::DEGREE360 * i / 16)) );

          lines.add( DefaultColors::blue, offset + mappos + lastPos, offset + mappos + curPos );
          lastPos = curPos;
        }
      }

      const world::Route& way = mobj->way();
      if( !way.empty() )
      {
        Point lastPos = way[ way.step ];
        for( world::Route::size_type k = way.step+1; k < way.size(); k++ )
        {
          lines.add( DefaultColors::aliceBlue, offset + lastPos, offset + way[ k ] );
          lastPos = way[ k ];
        }
      }
#endif
    }
  }
}

void EmpireMapWindow::Impl::showTradeAdvisorWindow()
{
  GameEventPtr e = ShowAdvisorWindow::create( true, advisor::trading );
  e->dispatch();
}

void EmpireMapWindow::Impl::initBorder( Widget* p )
{
  bottonMargin = 120;
  Picture backgr( ResourceGroup::empirepnls, 4 );
  Pictures pics;
  for( unsigned int y=p->height() - bottonMargin; y < p->height(); y+=backgr.height() )
  {
    for( unsigned int x=0; x < p->width(); x += backgr.width() )
    {
      pics.append( backgr, Point( x, -y ) );
    }
  }

  Picture lrBorderPic( ResourceGroup::empirepnls, 1 );
  for( unsigned int y = 0; y < p->height(); y += lrBorderPic.height() )
  {
    pics.append( lrBorderPic, Point( 0, -y ) );
    pics.append( lrBorderPic, Point( p->width() - lrBorderPic.width(), -y ) );
  }

  Picture tdBorderPic( ResourceGroup::empirepnls, 2 );
  for( unsigned int x = 0; x < p->width(); x += tdBorderPic.width() )
  {
    pics.append( tdBorderPic, Point( x, 0 ) );
    pics.append( tdBorderPic, Point( x, -p->height() + tdBorderPic.height() ) );
    pics.append( tdBorderPic, Point( x, -p->height() + bottonMargin ) );
  }

  Picture corner( ResourceGroup::empirepnls, 3 );
  pics.append( corner, Point( 0, 0 ) );    //left top
  pics.append( corner, Point( 0, -p->height() + corner.height() ) ); //top right
  pics.append( corner, Point( p->width() - corner.width(), 0 ) ); //left bottom
  pics.append( corner, Point( p->width() - corner.width(), -p->height() + corner.height() ) ); //bottom right
  pics.append( corner, Point( 0, -p->height() + bottonMargin ) ); //left middle
  pics.append( corner, Point( p->width() - corner.width(), -p->height() + bottonMargin ) ); //right middle

  Picture leftEagle( ResourceGroup::empirepnls, 7 );
  Picture rightEagle( ResourceGroup::empirepnls, 8 );
  Picture centerPicture( ResourceGroup::empirepnls, 9 );
  Size eagleOffset = corner.size();

  pics.append( leftEagle, Point( eagleOffset.width(), -p->height() + (bottonMargin - eagleOffset.height() + leftEagle.height() + 10) ) );
  pics.append( rightEagle, Point( p->width() - eagleOffset.width() - rightEagle.width(),
                                    -p->height() + (bottonMargin - eagleOffset.height() + rightEagle.height() + 10) ) );

  pics.append( centerPicture, Point( (p->width() - centerPicture.width()) / 2,
                                      -p->height() + (bottonMargin + centerPicture.height() - 20)) );

  if( centerPicture.texture() == corner.texture() )
  {
    Decorator::reverseYoffset( pics );
    border.batch.load( pics, Point( 0, 0) );
  }
  else
    border.nobatch = pics;
}

void EmpireMapWindow::Impl::drawLines(Engine &painter)
{
  foreach( it, lines )
  {
    painter.drawLine( it->color, it->p1, it->p2 );
  }
}

void EmpireMapWindow::Impl::drawCell(Engine& e, Point start, int side, NColor color)
{
#ifdef DEBUG
  lines.add( color, start, start + Point( side, 0 ) );
  lines.add( color, start + Point( side, 0 ), start + Point( side, side ) );
  lines.add( color, start + Point( side, side ), start + Point( 0, side ) );
  lines.add( color, start + Point( 0, side ), start );
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
      GameEventPtr e = Payment::create( econ::Issue::sundries, -(int)cost );
      e->dispatch();

      DockList docks = city::statistic::getObjects<Dock>( city, object::dock );
      if( docks.empty() )
      {
        GameEventPtr e = ShowInfobox::create( _("##no_working_dock##" ), _( "##no_dock_for_sea_trade_routes##" ) );
        e->dispatch();
      }
    }
  }

  updateCityInfo();
}

void EmpireMapWindow::Impl::drawCityInfo()
{
  Label* lb = new Label( gbox, Rect( Point( 0, gbox->height() - 70), Size( gbox->width(), 30 ) ) );
  std::string text = currentCity->about( world::Object::empireAdvInfo );
  lb->setTextAlignment( align::center, align::upperLeft );
  lb->setText( _( text ) );
}

void EmpireMapWindow::Impl::drawCityGoodsInfo()
{
  Point startInfo( 0, 0 );
  Point startButton( 0, 40 );
  world::EmpirePtr empire = city->empire();

  Point startDraw( (gbox->width() - 400) / 2, gbox->height() - 90 );
  new Label( gbox, Rect( startDraw + startInfo, Size( 70, 30 )), _("##emw_sell##") );

  const good::Store& sellgoods = currentCity->sells();
  int k=0;
  foreach( product, good::all() )
  {
    if( sellgoods.capacity( *product ) > 0  )
    {
      Label* lb = new Label( gbox, Rect( startDraw + startInfo + Point( 30 * (k+2), 0 ), Size( 24, 24 ) ) );
      lb->setBackgroundPicture( good::Helper::picture( *product, true) );
      lb->setTooltipText( good::Helper::getTypeName( *product ) );
      k++;
    }
  }

  Point buyPoint = startDraw + Point( 200, 0 );
  new Label( gbox, Rect( buyPoint + startInfo, Size( 70, 30 )), _("##emw_buy##") );

  const good::Store& buygoods = currentCity->buys();
  k=0;
  foreach( product, good::all() )
  {
    if( buygoods.capacity( *product ) > 0  )
    {
      Label* lb = new Label( gbox, Rect( buyPoint + startInfo + Point( 30 * (k+2), 0 ), Size( 24, 24 ) ) );
      lb->setBackgroundPicture(  good::Helper::picture( *product, true) );
      lb->setTooltipText( good::Helper::getTypeName( *product ) );
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

  const good::Store& sellgoods = currentCity->sells();
  int k=0;
  foreach( i, good::all() )
  {
    Unit maxsell = Unit::fromQty( sellgoods.capacity( *i ) );
    Unit cursell = Unit::fromQty( sellgoods.qty( *i ) );
    if( maxsell > 0  )
    {
      Label* lb = new Label( gbox, Rect( startDraw + Point( 80 + 100 * k, 0 ), Size( 24, 24 ) ) );
      lb->setBackgroundPicture(  good::Helper::picture( *i, true) );

      std::string text = utils::format( 0xff, "%d/%d", cursell.ivalue(), maxsell.ivalue() );
      new Label( gbox, Rect( startDraw + Point( 110 + 100 * k, 0), Size( 70, 30 ) ), text );
      k++;
    }
  }

  Point buyPoint = startDraw + Point( 0, 30 );
  new Label( gbox, Rect( buyPoint, Size( 80, 30 )), _("##emw_bought##") );

  const good::Store& buygoods = currentCity->buys();
  k=0;
  foreach( i, good::all() )
  {
    Unit maxbuy = Unit::fromQty( buygoods.capacity( *i ) );
    Unit curbuy = Unit::fromQty( buygoods.qty( *i ) );
    if( maxbuy > 0  )
    {
      Label* lb = new Label( gbox, Rect( buyPoint + Point( 80 + 100 * k, 0 ), Size( 24, 24 ) ) );
      lb->setBackgroundPicture( good::Helper::picture( *i, true) );

      std::string text = utils::format( 0xff, "%d/%d", curbuy.ivalue(), maxbuy.ivalue() );
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
  Dialog* dialog = Confirmation( gbox->ui(),
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
  _d->empireMap.load( "the_empire", 1 );
  _d->drag.active = false;
  GET_DWIDGET_FROM_UI( _d, lbTitle )

  _d->offset = game::Settings::get( empMapOffset ).toPoint();

  WidgetEscapeCloser::insertTo( this );

  _d->initBorder( this );

  GET_DWIDGET_FROM_UI( _d, gbox )
  if( _d->gbox ) _d->gbox->sendToBack();

  GET_DWIDGET_FROM_UI( _d, btnHelp )
  GET_DWIDGET_FROM_UI( _d, btnExit )
  GET_DWIDGET_FROM_UI( _d, btnTrade )
  GET_DWIDGET_FROM_UI( _d, btnAi )

  CONNECT( _d->btnExit, onClicked(), this, EmpireMapWindow::deleteLater )
  CONNECT( _d->btnTrade, onClicked(), this, EmpireMapWindow::deleteLater )
  CONNECT( _d->btnTrade, onClicked(), _d.data(), Impl::showTradeAdvisorWindow )
  CONNECT( _d->btnHelp, onClicked(), this, EmpireMapWindow::_showHelp )
  CONNECT( _d->btnAi, onClicked(), this, EmpireMapWindow::_toggleAi )

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

  _d->drawLines( engine );

  _d->border.batch.valid()
    ? engine.draw( _d->border.batch, &absoluteClippingRectRef() )
    : engine.draw( _d->border.nobatch, Point(), &absoluteClippingRectRef() );

  Widget::draw( engine );
}

void EmpireMapWindow::beforeDraw(Engine& painter)
{
  _d->lines.clear();
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
      _d->drag.start = event.mouse.pos();
      _d->drag.active = true;
      bringToFront();

      if( _d->flags.isFlag( showCityInfo ) )
        _d->checkCityOnMap( _d->drag.start );
    break;

    case mouseRbtnRelease:
      deleteLater();
      _d->drag.active = false;
    break;

    case mouseLbtnRelease:
      _d->drag.active = false;
    break;

    case mouseMoved:
    {
      if ( !event.mouse.isLeftPressed() )
      {
        _d->drag.active = false;
      }

      if( _d->drag.last != event.mouse.pos() )
      {
        _d->drag.last = event.mouse.pos();
        _changePosition();
      }

      if( _d->drag.active )
      {
        // gui window should not be dragged outside its parent
        bool mouseOutsideLeftup = (_d->offset.x() > 0 || _d->offset.y() > 0 );
        bool mouseXlessRight = (_d->offset.x() + _d->empireMap.width()) < (int)width();
        bool mouseYLessBottom = (_d->offset.y() + _d->empireMap.height()) < ((int)height()- (int)_d->bottonMargin);

        if( mouseOutsideLeftup || mouseXlessRight || mouseYLessBottom )
        {
          break;
        }

        _d->offset += (event.mouse.pos() - _d->drag.start );
        _d->drag.start = event.mouse.pos();

        _d->offset.setX( math::clamp<int>( _d->offset.x(), -_d->empireMap.width() + width(), 0 ) );
        _d->offset.setY( math::clamp<int>( _d->offset.y(), -_d->empireMap.height() + height() - _d->bottonMargin, 0 ) );
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
    text = obj->about( world::Object::empireMap );
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
    Label* elm = new Label( this, Rect( 0, 0, 2, 2 ), _(text), true, Label::bgSimpleWhite );
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

    Rect rect( _d->drag.last, tlpSize );

    rect -= Point( tlpSize.width() + 20, -20 );
    Rect pRect = parent()->absoluteRect();
    rect.constrainTo( pRect );

    elm->setGeometry( rect );

    _d->tooltipLabel = elm;
  }
}

const Point& EmpireMapWindow::_offset() const { return _d->offset; }
Widget* EmpireMapWindow::_resetInfoPanel() { _d->resetInfoPanel(); return _d->gbox; }
void EmpireMapWindow::_showHelp() { DictionaryWindow::show( this, "empiremap" ); }

void EmpireMapWindow::_toggleAi()
{
  if( _d->currentCity.isValid() )
  {
    world::City::AiMode mode = _d->currentCity->modeAI();
    _d->currentCity->setModeAI( mode == world::City::inactive
                                      ? world::City::indifferent
                                      : world::City::inactive );
    mode = _d->currentCity->modeAI();
    _d->btnAi->setText( mode == world::City::inactive ? "CC" : "AI" );
  }
}

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
