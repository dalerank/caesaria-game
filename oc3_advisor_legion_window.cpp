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

#include "oc3_advisor_legion_window.hpp"
#include "oc3_picture.hpp"
#include "oc3_gui_paneling.hpp"
#include "oc3_gettext.hpp"
#include "oc3_pushbutton.hpp"
#include "oc3_label.hpp"
#include "oc3_resourcegroup.hpp"
#include "oc3_stringhelper.hpp"
#include "oc3_gfx_engine.hpp"

class AdvisorLegionWindow::Impl
{
public:
  PictureRef background;
  Label* alarm;
  Label* helpRequest;
};

AdvisorLegionWindow::AdvisorLegionWindow( Widget* parent, int id ) 
: Widget( parent, id, Rect( 0, 0, 1, 1 ) ), _d( new Impl )
{
  setGeometry( Rect( Point( (parent->getWidth() - 640 )/2, parent->getHeight() / 2 - 242 ),
                     Size( 640, 416 ) ) );

  _d->background.reset( Picture::create( getSize() ) );
  //main background
  GuiPaneling::instance().draw_white_frame(*_d->background, 0, 0, getWidth(), getHeight() );

  //buttons background
  Point startPos( 32, 70 );
  GuiPaneling::instance().draw_black_frame(*_d->background, startPos.getX(), startPos.getY(), 574, 270 );

  Label* title = new Label( this, Rect( 10, 10, getWidth() - 10, 10 + 40) );
  title->setText( _("##advlegion_window_title##") );
  title->setFont( Font( FONT_3 ) );
  title->setTextAlignment( alignCenter, alignCenter );

  _d->alarm = new Label( this, Rect( 60, getHeight()-60, getWidth() - 60, getHeight() - 40 ), _("##advlegion_noalarm##") );
  _d->helpRequest = new Label( this, Rect( 60, getHeight()-40, getWidth() - 60, getHeight() - 20 ), _("##advlegion_norequest##") );
}

void AdvisorLegionWindow::draw( GfxEngine& painter )
{
  if( !isVisible() )
    return;

  painter.drawPicture( *_d->background, getScreenLeft(), getScreenTop() );

  Widget::draw( painter );
}