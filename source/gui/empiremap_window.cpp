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

#include "empiremap_window.hpp"
#include "gfx/picture.hpp"
#include "core/event.hpp"
#include "gfx/engine.hpp"
#include "texturedbutton.hpp"
#include "core/color.hpp"
#include "world/empire.hpp"
#include "world/computer_city.hpp"
#include "city/city.hpp"
#include "label.hpp"
#include "core/stringhelper.hpp"
#include "core/gettext.hpp"
#include "dialogbox.hpp"
#include "good/goodstore.hpp"
#include "world/trading.hpp"
#include "city/funds.hpp"
#include "good/goodhelper.hpp"
#include "game/settings.hpp"
#include "events/event.hpp"
#include "core/logger.hpp"
#include "world/merchant.hpp"
#include "core/foreach.hpp"
#include "world/object.hpp"
#include "events/fundissue.hpp"
#include "events/showadvisorwindow.hpp"

namespace gui
{

static const char* empMapOffset = "EmpireMapWindowOffset";

class EmpireMapWindow::Impl
{
public:
  PictureRef border;
  Picture empireMap;
  world::CityPtr currentCity;
  Point offset;
  bool dragging;
  Point dragStartPosition;
  Picture leftEagle, rightEagle;
  Size eagleOffset;
  Picture centerPicture;
  PushButton* btnHelp;
  PushButton* btnExit;
  PushButton* btnTrade;
  std::string ourCity;
  Picture citypics[4];
  Label* lbCityTitle;
  Widget* tradeInfo;
  world::EmpirePtr empire;

  void checkCityOnMap( const Point& pos );
  void showOpenRouteRequestWindow();
  void createTradeRoute();
  void drawCityGoodsInfo();
  void drawTradeRouteInfo();
  void drawCityInfo();
  void resetInfoPanel();
  void updateCityInfo();
  void showTradeAdvisorWindow();
};

void EmpireMapWindow::Impl::checkCityOnMap( const Point& pos )
{
  world::CityList cities = empire->getCities();

  currentCity = 0;
  foreach( city, cities )
  {
    Rect rect( (*city)->getLocation(), Size( 40 ) );
    if( rect.isPointInside( pos ) )
    {
      currentCity = (*city);
      break;
    }
  }

  updateCityInfo();
}

void EmpireMapWindow::Impl::updateCityInfo()
{
  resetInfoPanel();
  if( currentCity != 0 )
  {
    lbCityTitle->setText( currentCity->getName() );

    if( is_kind_of<PlayerCity>( currentCity ) )
    {
      drawCityInfo();
    }
    else
    {
      world::ComputerCityPtr ccity = ptr_cast<world::ComputerCity>( currentCity );

      if( ccity->isDistantCity() || ccity->isRomeCity() )
      {

      }
      else
      {
        world::TraderoutePtr route = empire->getTradeRoute( currentCity->getName(), ourCity );
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
    lbCityTitle->setText( "" );    
  }
}

void EmpireMapWindow::Impl::showTradeAdvisorWindow()
{
  events::GameEventPtr e = events::ShowAdvisorWindow::create( true, ADV_TRADING );
  e->dispatch();
}

void EmpireMapWindow::Impl::createTradeRoute()
{
  if( currentCity != 0 )
  {
    unsigned int cost = world::EmpireHelper::getTradeRouteOpenCost( empire, ourCity, currentCity->getName() );
    events::GameEventPtr e = events::FundIssueEvent::create( CityFunds::otherExpenditure, -(int)cost );
    e->dispatch();
    empire->createTradeRoute( ourCity, currentCity->getName() );
  }

  updateCityInfo();
}

void EmpireMapWindow::Impl::drawCityInfo()
{
  Label* lb = new Label( tradeInfo, Rect( Point( 0, tradeInfo->getHeight() - 70), Size( tradeInfo->getWidth(), 30) ) );
  lb->setTextAlignment( alignCenter, alignUpperLeft );
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
  Point startDraw( (tradeInfo->getWidth() - 400) / 2, tradeInfo->getHeight() - 90 );
  new Label( tradeInfo, Rect( startDraw, Size( 70, 30 )), _("##emw_sell##") );

  const GoodStore& sellgoods = currentCity->getSells();
  for( int i=0, k=0; i < Good::goodCount; i++ )
  {
    if( sellgoods.capacity( (Good::Type)i ) > 0  )
    {
      Label* lb = new Label( tradeInfo, Rect( startDraw + Point( 70 + 30 * k, 0 ), Size( 24, 24 ) ) );
      lb->setBackgroundPicture( GoodHelper::getPicture( Good::Type(i), true) );
      lb->setTooltipText( GoodHelper::getTypeName( Good::Type(i) ) );
      k++;
    }
  }

  Point buyPoint = startDraw + Point( 200, 0 );
  new Label( tradeInfo, Rect( buyPoint, Size( 70, 30 )), _("##emw_buy##") );

  const GoodStore& buygoods = currentCity->getBuys();
  for( int i=0, k=0; i < Good::goodCount; i++ )
  {
    if( buygoods.capacity( (Good::Type)i ) > 0  )
    {
      Label* lb = new Label( tradeInfo, Rect( buyPoint + Point( 70 + 30 * k, 0 ), Size( 24, 24 ) ) );
      lb->setBackgroundPicture(  GoodHelper::getPicture( Good::Type(i), true) );
      lb->setTooltipText( GoodHelper::getTypeName( Good::Type(i) ) );
      k++;
    }
  }

  PushButton* btnOpenTrade = new PushButton( tradeInfo, Rect( startDraw + Point( 0, 40 ), Size( 400, 20 ) ),
                                             "", -1, false, PushButton::blackBorderUp );

  unsigned int routeOpenCost = world::EmpireHelper::getTradeRouteOpenCost( empire, ourCity, currentCity->getName() );

  btnOpenTrade->setText( StringHelper::format( 0xff, "%d %s", routeOpenCost, _("##dn_for_open_trade##")));

  CONNECT( btnOpenTrade, onClicked(), this, Impl::showOpenRouteRequestWindow );
}

void EmpireMapWindow::Impl::drawTradeRouteInfo()
{
  Point startDraw( (tradeInfo->getWidth() - 400) / 2, tradeInfo->getHeight() - 80 );
  new Label( tradeInfo, Rect( startDraw, Size( 80, 30 )), _("##emw_sold##") );

  const GoodStore& sellgoods = currentCity->getSells();
  for( int i=0, k=0; i < Good::goodCount; i++ )
  {
    int maxsell = sellgoods.capacity( (Good::Type)i ) / 100;
    int cursell = sellgoods.getQty( (Good::Type)i ) / 100;
    if( maxsell > 0  )
    {
      Label* lb = new Label( tradeInfo, Rect( startDraw + Point( 80 + 100 * k, 0 ), Size( 24, 24 ) ) );
      lb->setBackgroundPicture(  GoodHelper::getPicture( Good::Type(i), true) );

      std::string text = StringHelper::format( 0xff, "%d/%d", cursell, maxsell );
      new Label( tradeInfo, Rect( startDraw + Point( 110 + 100 * k, 0), Size( 70, 30 ) ), text );
      k++;
    }
  }

  Point buyPoint = startDraw + Point( 0, 30 );
  new Label( tradeInfo, Rect( buyPoint, Size( 80, 30 )), _("##emw_bought##") );

  const GoodStore& buygoods = currentCity->getBuys();
  for( int i=0, k=0; i < Good::goodCount; i++ )
  {
    int maxbuy = buygoods.capacity( (Good::Type)i ) / 100;
    int curbuy = buygoods.getQty( (Good::Type)i ) / 100;
    if( maxbuy > 0  )
    {
      Label* lb = new Label( tradeInfo, Rect( buyPoint + Point( 80 + 100 * k, 0 ), Size( 24, 24 ) ) );
      lb->setBackgroundPicture( GoodHelper::getPicture( Good::Type(i), true) );

      std::string text = StringHelper::format( 0xff, "%d/%d", curbuy, maxbuy );
      new Label( tradeInfo, Rect( buyPoint + Point( 110 + 100 * k, 0), Size( 70, 30 ) ), text );
      k++;
    }
  }

/*  PushButton* btnOpenTrade = new PushButton( tradeInfo, Rect( startDraw, Size( 400, 20 ) ),
    "", -1, false, PushButton::blackBorderUp );
  btnOpenTrade->setText( StringHelper::format( 0xff, "%d %s", 1000, _("##dn_for_open_trade##")));
  */
}

void EmpireMapWindow::Impl::resetInfoPanel()
{
  Widget::Widgets childs = tradeInfo->getChildren();
  foreach( widget, childs ) { (*widget)->deleteLater(); }
}

void EmpireMapWindow::Impl::showOpenRouteRequestWindow()
{
  DialogBox* dialog = new DialogBox( tradeInfo->getParent(), Rect( 0, 0, 0, 0 ), 
                                     _("##emp_open_trade_route##"), _("##emp_pay_open_this_route_question##"), 
                                     DialogBox::btnOk | DialogBox::btnCancel  );

  CONNECT( dialog, onOk(), this, Impl::createTradeRoute );
  CONNECT( dialog, onOk(), dialog, DialogBox::deleteLater );
  CONNECT( dialog, onCancel(), dialog, DialogBox::deleteLater );
}

EmpireMapWindow::EmpireMapWindow( Widget* parent, int id )
 : Widget( parent, id, Rect( Point(0, 0), parent->getSize() ) ), _d( new Impl )
{
  // use some clipping to remove the right and bottom areas
  _d->border.reset( Picture::create( getSize() ) );
  _d->empireMap = Picture::load( "the_empire", 1 );
  _d->dragging = false;
  _d->lbCityTitle = new Label( this, Rect( Point( (getWidth() - 240) / 2 + 60, getHeight() - 132 ), Size( 240, 32 )) );
  _d->lbCityTitle->setFont( Font::create( FONT_3 ) );
  _d->offset = GameSettings::get( empMapOffset ).toPoint();

  _d->tradeInfo = new Widget( this, -1, Rect( 0, getHeight() - 120, getWidth(), getHeight() ) );

  const Picture& backgr = Picture::load( ResourceGroup::empirepnls, 4 );
  for( unsigned int y=getHeight() - 120; y < getHeight(); y+=backgr.getHeight() )
  {
    for( unsigned int x=0; x < getWidth(); x += backgr.getWidth() )
    {
      _d->border->draw( backgr, x, y );
    }
  }

  const Picture& lrBorderPic = Picture::load( ResourceGroup::empirepnls, 1 );
  for( unsigned int y = 0; y < getHeight(); y += lrBorderPic.getHeight() )
  {
    _d->border->draw( lrBorderPic, 0, y );
    _d->border->draw( lrBorderPic, getWidth() - lrBorderPic.getWidth(), y );
  }

  const Picture& tdBorderPic = Picture::load( ResourceGroup::empirepnls, 2 );
  for( unsigned int x = 0; x < getWidth(); x += tdBorderPic.getWidth() )
  {
    _d->border->draw( tdBorderPic, x, 0 );
    _d->border->draw( tdBorderPic, x, getHeight() - tdBorderPic.getHeight() );
    _d->border->draw( tdBorderPic, x, getHeight() - 120 );
  }

  const Picture& corner = Picture::load( ResourceGroup::empirepnls, 3 );
  _d->border->draw( corner, 0, 0 );    //left top
  _d->border->draw( corner, 0, getHeight() - corner.getHeight() ); //top right
  _d->border->draw( corner, getWidth() - corner.getWidth(), 0 ); //left bottom
  _d->border->draw( corner, getWidth() - corner.getWidth(), getHeight() - corner.getHeight() ); //bottom right
  _d->border->draw( corner, 0, getHeight() - 120 ); //left middle
  _d->border->draw( corner, getWidth() - corner.getWidth(), getHeight() - 120 ); //right middle

  _d->border->fill( 0x00000000, Rect( corner.getWidth(), corner.getHeight(), 
                                      getWidth() - corner.getWidth(), getHeight() - 120 ) );

  _d->leftEagle = Picture::load( ResourceGroup::empirepnls, 7 );
  _d->rightEagle = Picture::load( ResourceGroup::empirepnls, 8 );
  _d->eagleOffset = corner.getSize();

  _d->centerPicture = Picture::load( ResourceGroup::empirepnls, 9 );

  _d->btnHelp = new TexturedButton( this, Point( 20, getHeight() - 44 ), Size( 24 ), -1, 528 );
  _d->btnExit = new TexturedButton( this, Point( getWidth() - 44, getHeight() - 44 ), Size( 24 ), -1, 533 );
  _d->btnTrade = new TexturedButton( this, Point( getWidth() - 48, getHeight() - 100), Size( 28 ), -1, 292 );
  _d->btnTrade->setTooltipText( _("##to_trade_advisor##") );

  _d->citypics[ 0 ] = Picture::load( ResourceGroup::empirebits, 1 );
  _d->citypics[ 1 ] = Picture::load( ResourceGroup::empirebits, 8 );
  _d->citypics[ 2 ] = Picture::load( ResourceGroup::empirebits, 15 );
  _d->citypics[ 3 ] = Picture::load( ResourceGroup::empirebits, 22 );

  CONNECT( _d->btnExit, onClicked(), this, EmpireMapWindow::deleteLater );
  CONNECT( _d->btnTrade, onClicked(), this, EmpireMapWindow::deleteLater );
  CONNECT( _d->btnTrade, onClicked(), _d.data(), Impl::showTradeAdvisorWindow );
}

void EmpireMapWindow::draw( GfxEngine& engine )
{
  if( !isVisible() )
    return;

  engine.drawPicture( _d->empireMap, _d->offset );  

  world::CityList cities = _d->empire->getCities();
  foreach( city, cities )
  {
    Point location = (*city)->getLocation();
    int index = 3;
    if( is_kind_of<PlayerCity>( *city ) )  //maybe it our city
    {
      index = 0;
    }
    else if( is_kind_of<world::ComputerCity>( *city ) )
    {
      index = 2;
      world::ComputerCityPtr ccity = ptr_cast<world::ComputerCity>( *city );
      if( ccity->isDistantCity() )      {        index = 3;       }
      else if( ccity->isRomeCity() )       {        index = 1;      }
    }

    engine.drawPicture( _d->citypics[ index ], _d->offset + location );
  }  

  world::ObjectList objects = _d->empire->getObjects();
  foreach( obj, objects )
  {
    Point location = (*obj)->getLocation();
    engine.drawPicture( (*obj)->getPicture(), _d->offset +  location );
  }

  world::TraderouteList routes = _d->empire->getTradeRoutes();
  foreach( it, routes )
  {
    world::TraderoutePtr route = *it;
    const Picture& picture = Picture::load( ResourceGroup::empirebits,
                                            route->isSeaRoute() ? PicID::seaTradeRoute : PicID::landTradeRoute );

    world::MerchantPtr merchant = route->getMerchant( 0 );
    const PointsArray& points = route->getPoints();
    const PicturesArray& pictures = route->getPictures();
    for( unsigned int index=0; index < pictures.size(); index++ )
    {
      engine.drawPicture( pictures[ index ], _d->offset + points[ index ] );
    }

    if( merchant != 0 )
    {
      engine.drawPicture( picture, _d->offset + merchant->getLocation() );
    }      
  }

  engine.drawPicture( *_d->border, Point( 0, 0 ) );

  engine.drawPicture( _d->leftEagle, _d->eagleOffset.width(), getHeight() - 120 + _d->eagleOffset.height() - _d->leftEagle.getHeight() - 10 );
  engine.drawPicture( _d->rightEagle, getWidth() - _d->eagleOffset.width() - _d->rightEagle.getWidth(),
                      getHeight() - 120 + _d->eagleOffset.height() - _d->rightEagle.getHeight() - 10 );

  engine.drawPicture( _d->centerPicture, (getWidth() - _d->centerPicture.getWidth()) / 2,
                      getHeight() - 120 - _d->centerPicture.getHeight() + 20 );

  Widget::draw( engine );
}

bool EmpireMapWindow::onEvent( const NEvent& event )
{
  if( event.EventType == sEventMouse )
  {
    switch(event.mouse.type)
    {
    case mouseLbtnPressed:
      _d->dragStartPosition = event.mouse.getPosition();
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
              || _d->offset.x() + _d->empireMap.getWidth() < (int)getWidth()
              || _d->offset.y() > 0
              || _d->offset.y() + _d->empireMap.getHeight() < (int)getHeight()-120 )
          {
            break;
          }

          _d->offset += (event.mouse.getPosition() - _d->dragStartPosition);
          _d->dragStartPosition = event.mouse.getPosition();

          _d->offset.setX( math::clamp<int>( _d->offset.x(), -_d->empireMap.getWidth() + getWidth(), 0 ) );
          _d->offset.setY( math::clamp<int>( _d->offset.y(), -_d->empireMap.getHeight() + getHeight() - 120, 0 ) );
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

EmpireMapWindow* EmpireMapWindow::create(world::EmpirePtr empire, PlayerCityPtr city, Widget* parent, int id )
{
  EmpireMapWindow* ret = new EmpireMapWindow( parent, id );
  ret->_d->empire = empire;
  ret->_d->ourCity = city->getName();

  return ret;
}

EmpireMapWindow::~EmpireMapWindow()
{
  GameSettings::set( empMapOffset, _d->offset );
}

}//end namespace gui
