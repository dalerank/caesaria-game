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

#include <cstdio>

#include "info_box.hpp"
#include "widget_helper.hpp"
#include "gameautopause.hpp"
#include "widgetescapecloser.hpp"
#include "core/logger.hpp"
#include "label.hpp"
#include "core/event.hpp"
#include "core/variant_map.hpp"
#include "stretch_layout.hpp"
#include "core/utils.hpp"
#include "core/gettext.hpp"
#include "game/resourcegroup.hpp"

using namespace gfx;

namespace gui
{

namespace infobox
{

const Rect Infobox::defaultRect = Rect( 0, 0, 510, 300 );

class Infobox::Impl
{
public:
  Label* lbBlackFrame;
  Label* lbTitle;
  Label* lbText;
  PushButton* btnExit;
  PushButton* btnHelp;
  bool isAutoPosition;
  std::map<Widget*, Callback> callbacks;

  Impl() : lbBlackFrame(0), lbTitle(0),
    lbText(0), btnExit(0), btnHelp(0),
    isAutoPosition(false)
  {

  }
};

Infobox::Infobox( Widget* parent, const Rect& rect, const Rect& blackArea, int id )
: Window( parent, rect, "", id ), _d( new Impl )
{
  GameAutoPauseWidget::insertTo( this );
  WidgetClosers::insertTo( this, KEY_RBUTTON );

  // create the title
  setupUI( ":/gui/infobox.gui" );

  GET_DWIDGET_FROM_UI( _d, lbTitle )
  GET_DWIDGET_FROM_UI( _d, btnExit )
  GET_DWIDGET_FROM_UI( _d, btnHelp )
  GET_DWIDGET_FROM_UI( _d, lbBlackFrame )
  GET_DWIDGET_FROM_UI( _d, lbText  )

  if( _d->btnExit )
  {
    _d->btnExit->setPosition( Point( width() - 39, height() - 39 ) );
    _d->btnExit->bringToFront();
  }

  if( _d->btnHelp )
  {
    _d->btnHelp->setPosition( Point( 14, height() - 39 ) );
    _d->btnHelp->bringToFront();
  }

  CONNECT( _d->btnExit, onClicked(), this, Infobox::deleteLater );
  CONNECT( _d->btnHelp, onClicked(), this, Infobox::_showHelp );

  // black box
  Point lastPos( width() - 32, height() - 48 );
  if( _d->lbBlackFrame )
  {
    _d->lbBlackFrame->setVisible( blackArea.size().area() > 0 );
    _d->lbBlackFrame->setGeometry( blackArea );
    lastPos.setY( _d->lbBlackFrame->top() - 10 );
  }

  if( _d->lbText )
  {
    Rect r = _d->lbText->relativeRect();
    if( blackArea.width() == 0 ) { r._bottomright = _d->btnExit->righttop(); }
    else { r._bottomright = blackArea.righttop(); }

    _d->lbText->setGeometry( r );
  }  

  _afterCreate();
  setModal();
}

void Infobox::setText( const std::string& text )
{
  if( _d->lbText ) { _d->lbText->setText( text ); }
}

Infobox::~Infobox() {}

void Infobox::draw(gfx::Engine& engine )
{
  Window::draw( engine );
}

bool Infobox::isPointInside( const Point& point ) const
{
  //resolve all screen for self using
  return parent()->absoluteRect().isPointInside( point );
}

bool Infobox::onEvent( const NEvent& event)
{
  switch( event.EventType )
  {
  case sEventMouse:
    if( event.mouse.type == NEvent::Mouse::mouseLbtnRelease )
    {
      return true;
    }
  break;

  default:
  break;
  }

  return Window::onEvent( event );
}

void Infobox::setTitle( const std::string& title )
{
  if( _d->lbTitle )
  {
    Size s = _d->lbTitle->font().getTextSize( title );
    if( s.width() > (int)_d->lbTitle->width() )
      _d->lbTitle->setFont( FONT_2 );

    _d->lbTitle->setText( title );
  }
}

bool Infobox::isAutoPosition() const{  return _d->isAutoPosition;}
void Infobox::setAutoPosition( bool value ){  _d->isAutoPosition = value;}

void Infobox::setupUI(const VariantMap& ui)
{
  Window::setupUI( ui );

  _d->isAutoPosition = ui.get( "autoPosition", true );
}

void Infobox::setupUI(const vfs::Path& filename)
{
  Window::setupUI( filename );
}

void Infobox::addCallback(const std::string& name, Callback callback)
{
  //Point start = _d->btnHelp->absoluteRect().righttop();
  INIT_WIDGET_FROM_UI( VLayout*, layout )
  if( layout )
  {
    auto& button = add<PushButton>( Rect(), name );
    button.onClicked() += callback;
  }
}

Label* Infobox::_lbTitle(){  return _d->lbTitle;}
Label* Infobox::_lbText(){ return _d->lbText; }
Label* Infobox::_lbBlackFrame(){  return _d->lbBlackFrame; }

PushButton*Infobox::_btnHelp() { return _d->btnHelp; }
PushButton*Infobox::_btnExit() { return _d->btnExit; }

void Infobox::_updateWorkersLabel(const Point &pos, int picId, int need, int have )
{
  _d->lbBlackFrame->setVisible( need > 0 );
  if( !_d->lbBlackFrame || 0 == need)
    return;

  // number of workers
  std::string text = fmt::format( "{} {} ({} {})",
                                  have, _("##employers##"),
                                  need, _("##requierd##") );
  _d->lbBlackFrame->setIcon( gui::rc.panel, picId );
  _d->lbBlackFrame->setIconOffset( { 20, 10 } );
  _d->lbBlackFrame->setText( text );
}


InfoboxBuilding::InfoboxBuilding( Widget* parent, const Tile& tile )
  : Infobox( parent, Rect( 0, 0, 450, 220 ), Rect( 16, 60, 450 - 16, 60 + 50) )
{
  auto building = tile.overlay<Building>();
  if( building.isValid() )
    setTitle( building->info().prettyName() );
}

}

}//end namespace gui
