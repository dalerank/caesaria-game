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

#include <memory>

#include "oc3_window_empiremap.hpp"
#include "oc3_picture.hpp"
#include "oc3_event.hpp"
#include "oc3_gfx_engine.hpp"
#include "oc3_texturedbutton.hpp"
#include "oc3_color.hpp"
#include "oc3_empire.hpp"
#include "oc3_empirecity.hpp"
#include "oc3_scenario.hpp"
#include "oc3_city.hpp"
#include "oc3_gui_label.hpp"
#include "oc3_gettext.hpp"

class EmpireMapWindow::Impl
{
public:
  PictureRef border;
  Picture empireMap;
  Scenario* scenario;
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
  Picture citypics[3];
  Label* lbCityTitle;
  Label* lbCityInfo;

  void checkCityOnMap( const Point& pos );
};

void EmpireMapWindow::Impl::checkCityOnMap( const Point& pos )
{
  const EmpireCities& cities = scenario->getEmpire()->getCities();

  EmpireCityPtr city;
  for( EmpireCities::const_iterator it=cities.begin(); it != cities.end(); it++ )
  {
    Rect rect( (*it)->getLocation(), Size( 40 ) );
    if( rect.isPointInside( pos ) )
    {
      city = *it;
      break;
    }
  }

  lbCityInfo->setVisible( city != 0 );
  if( city != 0 )
  {
    lbCityTitle->setText( city->getName() );

    if( city->getName() == scenario->getCity()->getName() )
    {
      lbCityInfo->setText( _("##empiremap_our_city##") );
    }
    else
    {
      lbCityInfo->setText( _("##empiremap_empire_city##") );
    }
  }
  else
  {
    lbCityTitle->setText( "" );    
  }
}

EmpireMapWindow::EmpireMapWindow( Widget* parent, int id )
 : Widget( parent, id, Rect( Point(0, 0), parent->getSize() ) ), _d( new Impl )
{
  // use some clipping to remove the right and bottom areas
  _d->border.reset( Picture::create( getSize() ) );
  _d->empireMap = Picture::load( "the_empire", 1 );
  _d->dragging = false;
  _d->lbCityTitle = new Label( this, Rect( Point( (getWidth() - 240) / 2 + 60, getHeight() - 130 ), Size( 240, 32 )) );
  _d->lbCityTitle->setFont( Font::create( FONT_3 ) );

  _d->lbCityInfo = new Label( this, Rect( Point( (getWidth() - 240)/2, getHeight() - 50), Size( 240, 32)) );

  const Picture& backgr = Picture::load( "empire_panels", 4 );
  for( unsigned int y=getHeight() - 120; y < getHeight(); y+=backgr.getHeight() )
  {
    for( unsigned int x=0; x < getWidth(); x += backgr.getWidth() )
    {
      _d->border->draw( backgr, x, y );
    }
  }

  const Picture& lrBorderPic = Picture::load( "empire_panels", 1 );
  for( unsigned int y = 0; y < getHeight(); y += lrBorderPic.getHeight() )
  {
    _d->border->draw( lrBorderPic, 0, y );
    _d->border->draw( lrBorderPic, getWidth() - lrBorderPic.getWidth(), y );
  }

  const Picture& tdBorderPic = Picture::load( "empire_panels", 2 );
  for( unsigned int x = 0; x < getWidth(); x += tdBorderPic.getWidth() )
  {
    _d->border->draw( tdBorderPic, x, 0 );
    _d->border->draw( tdBorderPic, x, getHeight() - tdBorderPic.getHeight() );
    _d->border->draw( tdBorderPic, x, getHeight() - 120 );
  }

  const Picture& corner = Picture::load( "empire_panels", 3 );
  _d->border->draw( corner, 0, 0 );    //left top
  _d->border->draw( corner, 0, getHeight() - corner.getHeight() ); //top right
  _d->border->draw( corner, getWidth() - corner.getWidth(), 0 ); //left bottom
  _d->border->draw( corner, getWidth() - corner.getWidth(), getHeight() - corner.getHeight() ); //bottom right
  _d->border->draw( corner, 0, getHeight() - 120 ); //left middle
  _d->border->draw( corner, getWidth() - corner.getWidth(), getHeight() - 120 ); //right middle

  _d->border->fill( 0x00000000, Rect( corner.getWidth(), corner.getHeight(), 
                                      getWidth() - corner.getWidth(), getHeight() - 120 ) );

  _d->leftEagle = Picture::load( "empire_panels", 7 );
  _d->rightEagle = Picture::load( "empire_panels", 8 );
  _d->eagleOffset = corner.getSize();

  _d->centerPicture = Picture::load( "empire_panels", 9 );

  _d->btnHelp = new TexturedButton( this, Point( 20, getHeight() - 44 ), Size( 24 ), -1, 528 );
  _d->btnExit = new TexturedButton( this, Point( getWidth() - 44, getHeight() - 44 ), Size( 24 ), -1, 533 );
  _d->btnTrade = new TexturedButton( this, Point( getWidth() - 48, getHeight() - 100), Size( 28 ), -1, 292 );

  _d->citypics[ 0 ] = Picture::load( ResourceGroup::empirebits, 1 );
  _d->citypics[ 1 ] = Picture::load( ResourceGroup::empirebits, 8 );
  _d->citypics[ 2 ] = Picture::load( ResourceGroup::empirebits, 15 );

  CONNECT( _d->btnExit, onClicked(), this, EmpireMapWindow::deleteLater );
  CONNECT( _d->btnTrade, onClicked(), this, EmpireMapWindow::deleteLater );
}

void EmpireMapWindow::draw( GfxEngine& engine )
{
  if( !isVisible() )
    return;

  engine.drawPicture( _d->empireMap, _d->offset );  

  const EmpireCities& cities = _d->scenario->getEmpire()->getCities();
  for( EmpireCities::const_iterator it=cities.begin(); it != cities.end(); it++ )
  {
    Point location = (*it)->getLocation();
    int index = 0; //maybe it our city
    if( (*it)->getName() != _d->ourCity )   
    {
      index = (*it)->isDistantCity() ? 1 : 2;
    }

    engine.drawPicture( _d->citypics[ index ], _d->offset + Point( location.getX(), location.getY() ) );
  }

  engine.drawPicture( *_d->border, Point( 0, 0 ) );

  engine.drawPicture( _d->leftEagle, _d->eagleOffset.getWidth(), getHeight() - 120 + _d->eagleOffset.getHeight() - _d->leftEagle.getHeight() - 10 );
  engine.drawPicture( _d->rightEagle, getWidth() - _d->eagleOffset.getWidth() - _d->rightEagle.getWidth(), 
    getHeight() - 120 + _d->eagleOffset.getHeight() - _d->rightEagle.getHeight() - 10 );

  engine.drawPicture( _d->centerPicture, (getWidth() - _d->centerPicture.getWidth()) / 2, 
    getHeight() - 120 - _d->centerPicture.getHeight() + 20 );

  Widget::draw( engine );
}

bool EmpireMapWindow::onEvent( const NEvent& event )
{
  if( event.EventType == OC3_MOUSE_EVENT )
  {
    switch(event.MouseEvent.Event)
    {
    case OC3_LMOUSE_PRESSED_DOWN:
      _d->dragStartPosition = event.MouseEvent.getPosition();
      _d->dragging = true;//_d->flags.isFlag( draggable );
      bringToFront();

      _d->checkCityOnMap( _d->dragStartPosition - _d->offset );
    break;

    case OC3_RMOUSE_LEFT_UP:
      deleteLater();
      _d->dragging = false;
    break;

    case OC3_LMOUSE_LEFT_UP:
      _d->dragging = false;
    break;

    case OC3_MOUSE_MOVED:
      {
        //bool t = _d->dragging;

        if ( !event.MouseEvent.isLeftPressed() )
        {
          _d->dragging = false;
        }

        if( _d->dragging )
        {
          // gui window should not be dragged outside its parent
          if( _d->offset.getX() > 0
              || _d->offset.getX() + _d->empireMap.getWidth() < (int)getWidth()
              || _d->offset.getY() > 0
              || _d->offset.getY() + _d->empireMap.getHeight() < (int)getHeight()-120 )
          {
            break;
          }

          _d->offset += (event.MouseEvent.getPosition() - _d->dragStartPosition);
          _d->dragStartPosition = event.MouseEvent.getPosition();

          _d->offset.setX( math::clamp<int>( _d->offset.getX(), -_d->empireMap.getWidth() + getWidth(), 0 ) );
          _d->offset.setY( math::clamp<int>( _d->offset.getY(), -_d->empireMap.getHeight() + getHeight() - 120, 0 ) );
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

EmpireMapWindow* EmpireMapWindow::create( Scenario* scenario, Widget* parent, int id )
{
  EmpireMapWindow* ret = new EmpireMapWindow( parent, id );
  ret->_d->scenario = scenario;
  ret->_d->ourCity = scenario->getCity()->getName();

  return ret;
}

Signal0<>& EmpireMapWindow::onTradeAdvisorRequest()
{
  return _d->btnTrade->onClicked(); 
}
