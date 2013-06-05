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

#include "oc3_advisors_window.hpp"

#include "oc3_picture.hpp"
#include "oc3_pictureconverter.hpp"
#include "oc3_gfx_engine.hpp"
#include "oc3_event.hpp"

class AdvisorsWindow::Impl
{
public:
  PictureRef background;
  Point offset;
  PictureRef tabBg;
};

AdvisorsWindow::AdvisorsWindow( Widget* parent, int id )
: Widget( parent, id, Rect( Point(0, 0), parent->getSize() ) ), _d( new Impl )
{
  // use some clipping to remove the right and bottom areas
  _d->background.reset( Picture::create( getSize() ) );

  Picture& backgr = Picture::load( "senate", 1 );

  _d->background->draw( backgr, Rect( Point( 0, 0), backgr.getSize() ), Rect( Point( 0,0), getSize() ) ); 
}

void AdvisorsWindow::draw( GfxEngine& engine )
{
  if( !isVisible() )
    return;

  engine.drawPicture( *_d->background, Point( 0, 0 ) );

  Widget::draw( engine );
}

bool AdvisorsWindow::onEvent( const NEvent& event )
{
  if( event.EventType == OC3_MOUSE_EVENT && event.MouseEvent.Event == OC3_RMOUSE_LEFT_UP )
  {
    deleteLater();
    return true;
  }

  return Widget::onEvent( event );
}

AdvisorsWindow* AdvisorsWindow::create( Widget* parent, int id )
{
  return new AdvisorsWindow( parent, id );
}