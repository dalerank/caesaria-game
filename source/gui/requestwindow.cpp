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
//
// Copyright 2012-2014 Dalerank, dalerankn8@gmail.com

#include "city/city.hpp"
#include "requestwindow.hpp"
#include "game/settings.hpp"
#include "label.hpp"
#include "core/stringhelper.hpp"
#include "image.hpp"
#include "good/goodhelper.hpp"
#include "texturedbutton.hpp"
#include "core/gettext.hpp"
#include "core/logger.hpp"
#include "gameautopause.hpp"
#include "events/showadvisorwindow.hpp"
#include "game/gamedate.hpp"
#include "widget_helper.hpp"
#include "smkviewer.hpp"

using namespace constants;

namespace gui
{

class EmperrorRequestWindow::Impl
{
public:
  void openEmperrorAdvisor();
  GameAutoPause locker;
  std::string video;
};

EmperrorRequestWindow* EmperrorRequestWindow::create( Widget* parent, city::request::RequestPtr request,
                                                      bool mayExec, const std::string& video )
{
  EmperrorRequestWindow* ret = new EmperrorRequestWindow( parent, request );
  if( mayExec )
  {
    ret->setText( _( "##city_have_goods_for_request##") );    
  }
  ret->_d->video = video;

  return ret;
}

EmperrorRequestWindow::~EmperrorRequestWindow() {}

EmperrorRequestWindow::EmperrorRequestWindow( Widget* parent, city::request::RequestPtr request )
  : Window( parent, Rect( 0, 0, 480, 320 ), "" ), _d( new Impl )
{
  _d->locker.activate();

  std::string uiFile = _d->video.empty() ? ":/gui/request.gui" : ":/gui/request_video.gui";

  setupUI( uiFile );

  setCenter( parent->center() );

  city::request::RqGoodPtr gr = ptr_cast<city::request::RqGood>(request);
  if( gr.isValid() )
  {
    Label* lbQty;
    Image* imgIcon;
    Label* lbInterval;
    SmkViewer* smkViewer;
    GET_WIDGET_FROM_UI( lbQty )
    GET_WIDGET_FROM_UI( imgIcon )
    GET_WIDGET_FROM_UI( lbInterval )
    GET_WIDGET_FROM_UI( smkViewer )

    if( lbQty ) { lbQty->setText( StringHelper::format( 0xff, "%d", gr->qty() ) ); }
    if( imgIcon ) { imgIcon->setPicture( GoodHelper::picture( gr->goodType() )); }

    int month2Comply = GameDate::current().monthsTo( gr->finishedDate() );
    if( lbInterval ) { lbInterval->setText( StringHelper::format( 0xff, "%d %s", month2Comply, _( "##months_to_comply##") )); }

    if( smkViewer ) { smkViewer->setFilename( GameSettings::rcpath( _d->video ) ); }
  }

  TexturedButton* btnExit;
  TexturedButton* btnAdvisor;
  GET_WIDGET_FROM_UI( btnExit )
  GET_WIDGET_FROM_UI( btnAdvisor )

  CONNECT( btnExit, onClicked(), this, EmperrorRequestWindow::deleteLater );
  CONNECT( btnAdvisor, onClicked(), _d.data(), Impl::openEmperrorAdvisor );
  CONNECT( btnAdvisor, onClicked(), this, EmperrorRequestWindow::deleteLater );
}

void EmperrorRequestWindow::draw(gfx::Engine& painter )
{
  if( !visible() )
    return;

  Window::draw( painter );
}

void EmperrorRequestWindow::setText(const std::string& text)
{
  Label* lbText;
  GET_WIDGET_FROM_UI( lbText )
  if( lbText )   {  lbText->setText( text );  }
}

bool EmperrorRequestWindow::onEvent(const NEvent& event)
{
  return Widget::onEvent( event );
}

void EmperrorRequestWindow::Impl::openEmperrorAdvisor()
{
  events::GameEventPtr e = events::ShowAdvisorWindow::create( true, advisor::empire );
  e->dispatch();
}

}//end namespace gui
