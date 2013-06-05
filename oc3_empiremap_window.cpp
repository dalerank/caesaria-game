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

#include "oc3_empiremap_window.hpp"
#include "oc3_picture.hpp"
#include "oc3_pictureconverter.hpp"
#include "oc3_gfx_engine.hpp"
#include "oc3_event.hpp"

class EmpireMapWindow::Impl
{
public:
  Picture border;
  Picture empireMap;
  Point offset;
};

EmpireMapWindow::EmpireMapWindow( Widget* parent, int id )
 : Widget( parent, id, Rect( Point(0, 0), parent->getSize() ) ), _d( new Impl )
{
  // use some clipping to remove the right and bottom areas
  _d->border = GfxEngine::instance().createPicture( getWidth(), getHeight() );
  GfxEngine::instance().loadPicture( _d->border );
  _d->empireMap = Picture::load( "the_empire", 1 );

  Picture& backgr = Picture::load( "empire_panels", 4 );
  for( int y=getHeight() - 120; y < getHeight(); y+=backgr.getHeight() )
  {
    for( int x=0; x < getWidth(); x += backgr.getWidth() )
    {
      _d->border.draw( backgr, x, y );
    }
  }

  Picture& lrBorderPic = Picture::load( "empire_panels", 1 );
  for( int y = 0; y < getHeight(); y += lrBorderPic.getHeight() )
  {
    _d->border.draw( lrBorderPic, 0, y );
    _d->border.draw( lrBorderPic, getWidth() - lrBorderPic.getWidth(), y );
  }

  Picture& tdBorderPic = Picture::load( "empire_panels", 2 );
  for( int x = 0; x < getWidth(); x += tdBorderPic.getWidth() )
  {
    _d->border.draw( tdBorderPic, x, 0 );
    _d->border.draw( tdBorderPic, x, getHeight() - tdBorderPic.getHeight() );
    _d->border.draw( tdBorderPic, x, getHeight() - 120 );
  }

  Picture& corner = Picture::load( "empire_panels", 3 );
  _d->border.draw( corner, 0, 0 );    //left top
  _d->border.draw( corner, 0, getHeight() - corner.getHeight() ); //top right
  _d->border.draw( corner, getWidth() - corner.getWidth(), 0 ); //left bottom
  _d->border.draw( corner, getWidth() - corner.getWidth(), getHeight() - corner.getHeight() ); //bottom right
  _d->border.draw( corner, 0, getHeight() - 120 ); //left middle
  _d->border.draw( corner, getWidth() - corner.getWidth(), getHeight() - 120 ); //right middle

  PictureConverter::fill( _d->border, 0x000000ff, Rect( corner.getWidth(), corner.getHeight(), 
                                                        getWidth() - corner.getWidth(), getHeight() - 120 ) );
}

void EmpireMapWindow::draw( GfxEngine& engine )
{
  if( !isVisible() )
    return;

  engine.drawPicture( _d->empireMap, _d->offset );
  engine.drawPicture( _d->border, Point( 0, 0 ) );

  Widget::draw( engine );
}

bool EmpireMapWindow::onEvent( const NEvent& event )
{
  if( event.EventType == OC3_MOUSE_EVENT && event.MouseEvent.Event == OC3_RMOUSE_LEFT_UP )
  {
    deleteLater();
    return true;
  }

  return Widget::onEvent( event );
}

EmpireMapWindow* EmpireMapWindow::create( Widget* parent, int id )
{
  return new EmpireMapWindow( parent, id );
}