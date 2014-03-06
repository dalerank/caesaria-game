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

#include "advisor_legion_window.hpp"
#include "gfx/decorator.hpp"
#include "core/gettext.hpp"
#include "gui/pushbutton.hpp"
#include "gui/label.hpp"
#include "game/resourcegroup.hpp"
#include "core/stringhelper.hpp"
#include "gfx/engine.hpp"

namespace gui
{

class AdvisorLegionWindow::Impl
{
public:
  PictureRef background;
  gui::Label* alarm;
  gui::Label* helpRequest;
};

AdvisorLegionWindow::AdvisorLegionWindow( Widget* parent, int id ) 
: Widget( parent, id, Rect( 0, 0, 1, 1 ) ), _d( new Impl )
{
  setGeometry( Rect( Point( (parent->width() - 640 )/2, parent->height() / 2 - 242 ),
                     Size( 640, 416 ) ) );

  _d->background.reset( Picture::create( size() ) );
  //main background
  PictureDecorator::draw( *_d->background, Rect( Point( 0, 0 ), size() ), PictureDecorator::whiteFrame );

  //buttons background
  PictureDecorator::draw( *_d->background, Rect( Point( 32, 70 ), Size( 574, 270 )), PictureDecorator::blackFrame );

  gui::Label* title = new gui::Label( this, Rect( 10, 10, width() - 10, 10 + 40) );
  title->setText( _("##advlegion_window_title##") );
  title->setFont( Font::create( FONT_3 ) );
  title->setTextAlignment( alignCenter, alignCenter );

  _d->alarm = new gui::Label( this, Rect( 60, height()-60, width() - 60, height() - 40 ), _("##advlegion_noalarm##") );
  _d->helpRequest = new gui::Label( this, Rect( 60, height()-40, width() - 60, height() - 20 ), _("##advlegion_norequest##") );
}

void AdvisorLegionWindow::draw( GfxEngine& painter )
{
  if( !isVisible() )
    return;

  painter.drawPicture( *_d->background, screenLeft(), screenTop() );

  Widget::draw( painter );
}

}//end namespace gui
