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
#include "core/utils.hpp"
#include "image.hpp"
#include "good/helper.hpp"
#include "texturedbutton.hpp"
#include "core/gettext.hpp"
#include "core/logger.hpp"
#include "gameautopause.hpp"
#include "events/showadvisorwindow.hpp"
#include "game/gamedate.hpp"
#include "widget_helper.hpp"
#include "smkviewer.hpp"

using namespace events;

namespace gui
{

class EmperrorRequestWindow::Impl
{
public:  
  std::string video;
};

EmperrorRequestWindow* EmperrorRequestWindow::create( Widget* parent, city::RequestPtr request,
                                                      bool mayExec, const std::string& video )
{
  auto* requestWindow = new EmperrorRequestWindow(parent, request);
  if( mayExec )
  {
    requestWindow->setText( _( "##city_have_goods_for_request##") );
  }
  requestWindow->setVideo( video );

  return requestWindow;
}

EmperrorRequestWindow::~EmperrorRequestWindow() {}

EmperrorRequestWindow::EmperrorRequestWindow( Widget* parent, city::RequestPtr request )
  : Window( parent, Rect( 0, 0, 480, 320 ), "" ), _d( new Impl )
{
  GameAutoPauseWidget::insertTo( this );

  std::string uiFile = _d->video.empty() ? ":/gui/request.gui" : ":/gui/request_video.gui";

  setupUI( uiFile );

  city::request::RqGoodPtr gr = ptr_cast<city::GoodRequest>(request);
  if( gr.isValid() )
  {
    INIT_WIDGET_FROM_UI(Label*, lbQty )
    INIT_WIDGET_FROM_UI(Label*, lbText )
    INIT_WIDGET_FROM_UI(Image*, imgIcon )
    INIT_WIDGET_FROM_UI(Label*, lbInterval )
    INIT_WIDGET_FROM_UI(SmkViewer*, smkViewer )
    INIT_WIDGET_FROM_UI(Label*, lbTitle )

    if( lbQty ) { lbQty->setText( utils::i2str( gr->qty() ) ); }
    if( imgIcon ) { imgIcon->setPicture( gr->info().picture() ); }

    std::string title, text, video;
    if( gr->info().type() == good::denaries )
    {
      text = "##rome_need_some_goods##";
      title = "##emperor_request_money##";
      video = "urgent_message1";
    }
    else
    {
      text = "##rome_need_some_money##";
      title = "##emperor_request##";
      video = "urgent_message2";
    }

    if( lbText )
    {
      lbText->setWordwrap( true );
      lbText->setText( _( text ) );
    }
    if( lbTitle ) { lbTitle->setText( _( title ) ); }

    int month2Comply = game::Date::current().monthsTo( gr->finishedDate() );
    if( lbInterval ) { lbInterval->setText( fmt::format( "{} {}", month2Comply, _( "##months_to_comply##") ) ); }

    video = _d->video.empty() ? video : _d->video;
    if( smkViewer ) { smkViewer->setFilename( video ); }
  }

  LINK_WIDGET_LOCAL_ACTION( TexturedButton*, btnExit,    onClicked(), EmperrorRequestWindow::deleteLater )
  LINK_WIDGET_LOCAL_ACTION( TexturedButton*, btnAdvisor, onClicked(), EmperrorRequestWindow::deleteLater )
  LINK_WIDGET_LOCAL_ACTION( TexturedButton*, btnAdvisor, onClicked(), EmperrorRequestWindow::_openEmperrorAdvisor )

  moveToCenter();
  setModal();
}

void EmperrorRequestWindow::draw(gfx::Engine& painter )
{
  if( !visible() )
    return;

  Window::draw( painter );
}

void EmperrorRequestWindow::setText(const std::string& text)
{
  INIT_WIDGET_FROM_UI( Label*, lbText )
  if( lbText ) { lbText->setText( text );  }
}

void EmperrorRequestWindow::setTitle(const std::string& text)
{
  INIT_WIDGET_FROM_UI( Label*, lbTitle )
  if( lbTitle ) { lbTitle->setText( text );  }
}

bool EmperrorRequestWindow::onEvent(const NEvent& event)
{
  return Widget::onEvent( event );
}

void EmperrorRequestWindow::setVideo(const std::string& video)
{
  _d->video = video;
}

void EmperrorRequestWindow::_openEmperrorAdvisor()
{
  events::dispatch<ShowAdvisorWindow>( true, advisor::empire );
}

}//end namespace gui
