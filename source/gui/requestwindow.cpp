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

#include "requestwindow.hpp"
#include "game/settings.hpp"
#include "label.hpp"
#include "core/stringhelper.hpp"
#include "image.hpp"
#include "good/goodhelper.hpp"
#include "texturedbutton.hpp"
#include "core/gettext.hpp"
#include "core/logger.hpp"

namespace gui
{

class EmperrorRequestWindow::Impl
{
public:
  void openEmperrorAdvisor();
};

EmperrorRequestWindow* EmperrorRequestWindow::create( Widget* parent, CityRequestPtr request )
{
  Size size( 625, 320 );

  Rect rectangle( Point( (parent->getWidth() - size.getWidth())/2, (parent->getHeight() - size.getHeight())/2 ), size );
  EmperrorRequestWindow* ret = new EmperrorRequestWindow( parent, request );

  return ret;
}

EmperrorRequestWindow::~EmperrorRequestWindow()
{

}

EmperrorRequestWindow::EmperrorRequestWindow( Widget* parent, CityRequestPtr request )
  : Widget( parent, -1, Rect( 0, 0, 1, 1 ) ), _d( new Impl )
{
  setupUI( GameSettings::rcpath( "/gui/request.gui" ) );

  setPosition( Point( parent->getWidth() - getWidth(), parent->getHeight() - getHeight() ) / 2 );

  if( request.is<GoodRequest>() )
  {
    GoodRequestPtr gr = request.as<GoodRequest>();

    Label* lb = findChild<Label*>( "lbQty", true );
    if( lb ) { lb->setText( StringHelper::format( 0xff, "%d", gr->getQty() ) ); }

    Image* img = findChild<Image*>( "imgIcon", true );
    if( img ) { img->setPicture( GoodHelper::getPicture( gr->getGoodType() )); }

    lb = findChild<Label*>( "lbInterval", true );
    if( lb ) { lb->setText( StringHelper::format( 0xff, "%d %s", gr->getMonths2Comply(), _( "##months_to_comply##") )); }
  }

  TexturedButton* btnExit = findChild<TexturedButton*>( "btnExit", true );
  TexturedButton* btnAdvisor = findChild<TexturedButton*>( "btnAdvisor", true );
  CONNECT( btnExit, onClicked(), this, EmperrorRequestWindow::deleteLater );
  CONNECT( btnAdvisor, onClicked(), _d.data(), Impl::openEmperrorAdvisor );
}

void EmperrorRequestWindow::draw( GfxEngine& painter )
{
  if( !isVisible() )
    return;

  Widget::draw( painter );
}

bool EmperrorRequestWindow::onEvent(const NEvent& event)
{
  return Widget::onEvent( event );
}

void EmperrorRequestWindow::Impl::openEmperrorAdvisor()
{

}

}//end namespace gui
