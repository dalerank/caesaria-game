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
#include "core/color_list.hpp"
#include "world/empire.hpp"
#include "world/computer_city.hpp"
#include "city/statistic.hpp"
#include "gfx/decorator.hpp"
#include "core/line.hpp"
#include "label.hpp"
#include "core/utils.hpp"
#include "world/empiremap.hpp"
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
#include "core/spring.hpp"
#include "core/metric.hpp"
#include "core/saveadapter.hpp"
#include "game/resourcegroup.hpp"
#include "city/states.hpp"
#include "gfx/maskstate.hpp"

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

class EmpireMapWindow::Impl
{
public:
  FlagHolder<int> flags;

  struct {
    Batch batch;
    Pictures nobatch;
  } border;

  struct {
    world::CityPtr current;
    PlayerCityPtr  base;
  } city;

  bool editorMode;
  Dragging drag;
  Picture empireMap;
  Point offset;
  Label* tooltipLabel;
  Lines lines;
  Widget* gbox;
  math::SpringI highlight;
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
  void drawDebugTiles( Engine& painter );
  void drawCell(Engine& e, Point start, int side , NColor color);
  world::ObjectPtr findObject( Point pos );

  Signal1<const std::string&> setTitleText;
};

void EmpireMapWindow::Impl::checkCityOnMap( const Point& pos )
{
  auto rcity = findObject( pos ).as<world::City>();

  if( rcity.isValid() )
  {
    city.current = rcity;
    updateCityInfo();
  }
}

void EmpireMapWindow::Impl::updateCityInfo()
{
  resetInfoPanel();
  if( city.current != 0 )
  {
    setTitleText( city.current->name() );

    if( is_kind_of<PlayerCity>( city.current ) )
    {
      drawCityInfo();
    }
    else
    {
      world::ComputerCityPtr ccity = ptr_cast<world::ComputerCity>( city.current );
      if( ccity.isNull() )
        return;

      if( ccity->isDistantCity() || ccity->states().romeCity )
      {

      }
      else
      {
        world::TraderoutePtr route = city.base->empire()->troutes().find( city.current->name(), city.base->name() );
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
    setTitleText( "" );
  }
}

void EmpireMapWindow::Impl::drawCities(Engine& painter)
{
  world::CityList cities = city.base->empire()->cities();
  Point location;
  Picture pic;

  highlight.update();

  for( auto pcity : cities )
  {
    MaskState mask( painter, pcity == city.current
                                ? highlight.value()
                                : ColorList::clear );
    location = pcity->location();
    pic = pcity->picture();
    painter.draw( pcity->pictures(), offset + location - Point( pic.width() / 2, pic.height() / 2 ) );

#ifdef DEBUG
    drawCell( painter, offset + location - Point( 10, 10 ), 20, ColorList::red );
#endif
  }
}

void EmpireMapWindow::Impl::drawStatic(Engine& painter)
{
  for( auto obj : city.base->empire()->objects() )
  {
    if( !obj->isMovable() )
    {
      painter.draw( obj->pictures(), offset + obj->location() );
    }
  }
}

void EmpireMapWindow::Impl::drawTradeRoutes(Engine& painter)
{
  world::TraderouteList routes = city.base->empire()->troutes().all();
  for( auto route : routes )
  {
    const PointsArray& points = route->points();
    const Pictures& pictures = route->pictures();

#ifdef DEBUG
    for( unsigned int index=1; index < pictures.size(); index++ )
    {
      Point pos1 = offset + points[ index-1 ];
      Point pos2 = offset + points[ index ];
      lines.add( ColorList::blue, pos1, pos2 );
      drawCell( painter, pos1 - Point( 10, 10 ), 20, ColorList::green );
    }
#endif

    MaskState mask( painter, route->isMyCity( city.current ) ? highlight.value() : 0 );

    for( unsigned int index=0; index < pictures.size(); index++ )
    {
      Point pos = offset + points[ index ];
      painter.draw( pictures[ index ], pos );
    }

    world::MerchantList merchants = route->merchants();
    for( auto merchant : merchants )
    {
      painter.draw( merchant->picture(), offset + merchant->location() );
    }
  }
}

void EmpireMapWindow::Impl::drawMovable(Engine& painter)
{
  for( auto obj : city.base->empire()->objects() )
  {
    if( obj->isMovable() )
    {
      auto movableObject = obj.as<world::MovableObject>();
      if( !movableObject.isValid() )
      {
        Logger::warning( "Object {} not movable", obj->name() );
        continue;
      }

      Point mappos = movableObject->location();
      painter.draw( movableObject->pictures(), offset + movableObject->location() );

#ifdef DEBUG
      int distance = movableObject->viewDistance();
      if( distance > 0 )
      {
        Point lastPos( distance * sin( 0 ), distance * cos( 0 ) );
        for( int i=1; i <= 16; i++ )
        {
          Point curPos( distance * sin( math::DEGTORAD * (math::DEGREE360 * i / 16)),
                        distance * cos( math::DEGTORAD * (math::DEGREE360 * i / 16)) );

          lines.add( ColorList::blue, offset + mappos + lastPos, offset + mappos + curPos );
          lastPos = curPos;
        }
      }

      const world::Route& way = movableObject->way();
      if( !way.empty() )
      {
        Point lastPos = way[ way.step ];
        for( world::Route::size_type k = way.step+1; k < way.size(); k++ )
        {
          lines.add( ColorList::aliceBlue, offset + lastPos, offset + way[ k ] );
          lastPos = way[ k ];
        }
      }
#endif
    }
  }
}

void EmpireMapWindow::Impl::showTradeAdvisorWindow()
{
  events::dispatch<ShowAdvisorWindow>( true, advisor::trading );
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
  for( auto line : lines )
    painter.drawLine( line.color, line.begin, line.end );
}

void EmpireMapWindow::Impl::drawDebugTiles(Engine& painter)
{
  if( !KILLSWITCH( showEmpireMapTiles ) )
    return;

  const world::EmpireMap& map = city.base->empire()->map();
  Size mapSize = map.size();
  Rect screenRect( Point( 0, 0), painter.screenSize() );

  NColor terrainColor[] = { ColorList::black,
                            ColorList::blue,
                            ColorList::brown,
                            ColorList::red,
                            ColorList::white,
                            ColorList::black,
                            ColorList::black,
                            ColorList::black };

  for( auto& color : terrainColor )
    color.setAlpha( 0x80 );

  for( auto i=0; i < mapSize.width(); i++ )
    for( auto j=0; j < mapSize.height(); j++ )
    {
      TilePos tpos( i, j );
      world::EmpireMap::TerrainType type = map.getTerrainType( tpos );
      Rect area = map.area( tpos );

      if( screenRect.isRectCollided( area + offset ) )
        painter.fillRect( terrainColor[type], area + offset );
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
  world::ObjectList objs = city.base->empire()->findObjects( -offset + pos, 15 );
  return objs.empty() ? world::ObjectPtr() : objs.front();
}

void EmpireMapWindow::Impl::createTradeRoute()
{
  if( city.current != 0 )
  {
    world::EmpirePtr empire = city.base->empire();
    world::TraderoutePtr route = empire->createTradeRoute( city.base->name(), city.current->name() );

    if( city.base.isValid() && route.isValid() && route->isSeaRoute() )
    {
      unsigned int cost = empire->troutes().getRouteOpenCost( city.base->name(), city.current->name() );
      events::dispatch<Payment>( econ::Issue::sundries, -(int)cost );

      size_t docks_n = city.base->statistic().objects.count( object::dock );
      if( !docks_n )
      {
        events::dispatch<ShowInfobox>( _("##no_working_dock##" ), _( "##no_dock_for_sea_trade_routes##" ) );
      }
    }
  }

  updateCityInfo();
}

void EmpireMapWindow::Impl::drawCityInfo()
{
  Label& lb = gbox->add<Label>( Rect( Point( 0, gbox->height() - 70), Size( gbox->width(), 30 ) ) );
  std::string text = city.current->about( world::Object::empireAdvInfo );
  lb.setTextAlignment( align::center, align::upperLeft );
  lb.setText( _( text ) );
}

void EmpireMapWindow::Impl::drawCityGoodsInfo()
{
  Point startInfo( 0, 0 );
  Point startButton( 0, 40 );
  world::EmpirePtr empire = city.base->empire();

  Point startDraw( (gbox->width() - 400) / 2, gbox->height() - 90 );
  gbox->add<Label>( Rect( startDraw + startInfo, Size( 70, 30 )), _("##emw_sell##") );

  const good::Store& sellgoods = city.current->sells();
  int k=0;
  for( auto& product : good::all() )
  {
    if( sellgoods.capacity( product ) > 0  )
    {
      Label& lb = gbox->add<Label>( Rect( startDraw + startInfo + Point( 30 * (k+2), 0 ), Size( 24, 24 ) ) );
      lb.setBackgroundPicture( good::Helper::picture( product, true) );
      lb.setTooltipText( good::Helper::getTypeName( product ) );
      k++;
    }
  }

  Point buyPoint = startDraw + Point( 200, 0 );
  gbox->add<Label>( Rect( buyPoint + startInfo, Size( 70, 30 )), _("##emw_buy##") );

  const good::Store& buygoods = city.current->buys();
  k=0;
  for( auto& product : good::all() )
  {
    if( buygoods.capacity( product ) > 0  )
    {
      Label& lb = gbox->add<Label>( Rect( buyPoint + startInfo + Point( 30 * (k+2), 0 ), Size( 24, 24 ) ) );
      lb.setBackgroundPicture(  good::Helper::picture( product, true) );
      lb.setTooltipText( good::Helper::getTypeName( product ) );
      k++;
    }
  }

  PushButton& btnOpenTrade = gbox->add<PushButton>( Rect( startDraw + startButton, Size( 400, 20 ) ),
                                                    "", -1, false, PushButton::blackBorderUp );

  unsigned int routeOpenCost = empire->troutes().getRouteOpenCost( city.base->name(), city.current->name() );

  btnOpenTrade.setText( fmt::format( "{} {}", routeOpenCost, _("##dn_for_open_trade##")));

  CONNECT( &btnOpenTrade, onClicked(), this, Impl::showOpenRouteRequestWindow );
}

class GoodText : public Label
{
public:
  GoodText( Widget* parent, const Point& pos, good::Product type, int current, int maxv )
    : Label( parent, Rect( pos, Size(100,30) ) )
  {
    setText( fmt::format( "{}/{}", current, maxv ) );
    setTextOffset( Point( 30, 0 ) );
    setIcon( good::Helper::picture( type, true), Point( 3, 3 ) );
  }
};

void EmpireMapWindow::Impl::drawTradeRouteInfo()
{
  Point startDraw( (gbox->width() - 400) / 2, gbox->height() - 80 );
  gbox->add<Label>( Rect( startDraw, Size( 80, 30 )), _("##emw_sold##") );

  const good::Store& sellgoods = city.current->sells();
  int k=0;
  for( auto& product : good::all() )
  {
    Unit maxsell = Unit::fromQty( sellgoods.capacity( product ) );
    Unit cursell = Unit::fromQty( sellgoods.qty( product ) );
    if( maxsell > 0  )
    {
      gbox->add<GoodText>( startDraw + Point( 80 + 100 * k, 0 ),
                           product, cursell.ivalue(), maxsell.ivalue() );
      k++;
    }
  }

  Point buyPoint = startDraw + Point( 0, 30 );
  gbox->add<Label>( Rect( buyPoint, Size( 80, 30 ) ), _("##emw_bought##") );

  const good::Store& buygoods = city.current->buys();
  k=0;
  for( auto& product : good::all() )
  {
    Unit maxbuy = Unit::fromQty( buygoods.capacity( product ) );
    Unit curbuy = Unit::fromQty( buygoods.qty( product ) );
    if( maxbuy > 0  )
    {
      gbox->add<GoodText>( buyPoint + Point( 80 + 100 * k, 0 ),
                           product, curbuy.ivalue(), maxbuy.ivalue() );
      k++;
    }
  }
}

void EmpireMapWindow::Impl::resetInfoPanel()
{
  Widget::Widgets childs = gbox->children();
  for( auto child : childs )
    child->deleteLater();
}

void EmpireMapWindow::Impl::showOpenRouteRequestWindow()
{
  Dialog* dialog = Confirmation( gbox->ui(),
                                 _("##emp_open_trade_route##"),
                                 _("##emp_pay_open_this_route_question##"),
                                 makeDelegate( this, &Impl::createTradeRoute ) );
}

EmpireMapWindow::EmpireMapWindow(Widget* parent, int id, PlayerCityPtr city )
 : Widget( parent, id, Rect( Point(0, 0), parent->size() ) ), _d( new Impl )
{
  // use some clipping to remove the right and bottom areas
  setupUI( ":/gui/empirewnd.gui" );

  _d->city.base = city;
  _d->editorMode = false;
  _d->tooltipLabel = 0;
  _d->highlight.setCondition( 100, 254, 4 );
  _d->empireMap.load( "the_empire", 1 );
  _d->drag.active = false;

  _d->offset = game::Settings::get( empMapOffset ).toPoint();

  WidgetClose::insertTo( this );
  GameAutoPause::insertTo( this );

  _d->initBorder( this );

  GET_DWIDGET_FROM_UI( _d, gbox )
  if( _d->gbox ) _d->gbox->sendToBack();

  LINK_WIDGET_LOCAL_ACTION( PushButton*, btnTrade, onClicked(), EmpireMapWindow::deleteLater )
  LINK_WIDGET_ACTION( PushButton*, btnTrade, onClicked(), _d.data(), Impl::showTradeAdvisorWindow )
  LINK_WIDGET_LOCAL_ACTION( PushButton*, btnAi, onClicked(), EmpireMapWindow::_toggleAi )

  INIT_WIDGET_FROM_UI( Label*, lbTitle )
  if( lbTitle )
    _d->setTitleText.connect( lbTitle, &Label::setText );

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

  _d->drawDebugTiles( engine );

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
    case NEvent::Mouse::mouseRbtnRelease:
      deleteLater();
      _d->drag.active = false;
    break;

    case NEvent::Mouse::mouseLbtnRelease:
      _d->drag.active = false;

      if( _d->editorMode )
      {
        world::EmpireMap& empmap = const_cast<world::EmpireMap&>( _d->city.base->empire()->map() );
        TilePos tpos = empmap.point2location( -_d->offset + event.mouse.pos() );
        int type = empmap.getTerrainType( tpos );

        if( type == 0 )
          type = world::EmpireMap::trSea;

        type <<= 1;
        if( type > world::EmpireMap::trAny )
          type = world::EmpireMap::trSea;

        empmap.setTerrainType( tpos, (world::EmpireMap::TerrainType)type );
      }
    break;

    case NEvent::Mouse::moved:
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
  else if( event.EventType == sEventKeyboard )
  {
    if( KILLSWITCH(showEmpireMapTiles) )
    {
      if( event.keyboard.control && !event.keyboard.pressed )
      {
        switch( event.keyboard.key )
        {
        case KEY_KEY_E:
          _d->editorMode = !_d->editorMode;
        break;

        case KEY_KEY_S:
          if( _d->editorMode )
          {
            VariantMap data = _d->city.base->empire()->map().save();
            vfs::Path path4save = SETTINGS_RC_PATH( worldModel );
            config::save( data, path4save );
          }
        break;

        default: break;
        }
      }

    }
  }

  return Widget::onEvent( event );
}

void EmpireMapWindow::_changePosition()
{
  Point cursorPos = ui()->cursorPos() ;
  world::ObjectPtr obj = _d->findObject( cursorPos );

  std::string text;
  if( obj.isValid() )
  {
    text = obj->about( world::Object::empireMap );
  }
  else
  {
    world::EmpirePtr empire = _d->city.base->empire();
    world::TraderouteList routes = empire->troutes().all();

    for( auto route : routes )
    {
      if( route->containPoint( -_d->offset + cursorPos, 4 ) )
      {
        text = route->isSeaRoute() ? "##sea_route##" : "##land_route##";
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
    Label& elm = add<Label>( Rect( 0, 0, 2, 2 ), _(text), true, Label::bgSimpleWhite );
    elm.setSubElement(true);
    elm.setTextAlignment( align::upperLeft, align::upperLeft );
    elm.setTextOffset( Point( 5, 5 ) );

    Size tlpSize( elm.textWidth() + 20, elm.textHeight() + 2 );
    if( tlpSize.width() > width() * 0.75 )
    {
      tlpSize.setWidth( width() * 0.5 );
      tlpSize.setHeight( elm.textHeight() * 2 + 10 );
      elm.setWordwrap( true );
    }

    Rect rect( _d->drag.last, tlpSize );

    rect -= Point( tlpSize.width() + 20, -20 );
    Rect pRect = parent()->absoluteRect();
    rect.constrainTo( pRect );

    elm.setGeometry( rect );

    _d->tooltipLabel = &elm;
    }
}

bool EmpireMapWindow::_onMousePressed( const NEvent::Mouse& event)
{
  _d->drag.start = event.pos();
  _d->drag.active = true;
  bringToFront();

  if( _d->flags.isFlag( showCityInfo ) )
    _d->checkCityOnMap( _d->drag.start );

#ifdef DEBUG
  {
    std::string text = _d->city.current.isValid()
                          ? _d->city.current->name()
                          : "";
    Point rpoint = -_d->offset + _d->drag.start;
    _d->setTitleText( text + fmt::format( " [{},{}]", rpoint.x(), rpoint.y() ) );
  }
#endif

  return true;
}

const Point& EmpireMapWindow::_offset() const { return _d->offset; }
Widget* EmpireMapWindow::_resetInfoPanel() { _d->resetInfoPanel(); return _d->gbox; }

void EmpireMapWindow::_toggleAi()
{
  INIT_WIDGET_FROM_UI(PushButton*, btnAi)
  if( _d->city.current.isValid() && btnAi )
  {
    world::City::AiMode mode = _d->city.current->modeAI();
    _d->city.current->setModeAI( mode == world::City::inactive
                                      ? world::City::indifferent
                                      : world::City::inactive );
    mode = _d->city.current->modeAI();
    btnAi->setText( mode == world::City::inactive ? "CC" : "AI" );
  }
}

EmpireMapWindow* EmpireMapWindow::create(PlayerCityPtr city, Widget* parent, int id )
{
  return &parent->add<EmpireMapWindow>( id, city );
}

EmpireMapWindow::~EmpireMapWindow()
{
  game::Settings::set( empMapOffset, _d->offset );
}

}//end namespace gui
