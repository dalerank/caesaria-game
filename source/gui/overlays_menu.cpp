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
// Copyright 2012-2015 Dalerank, dalerankn8@gmail.com

#include "overlays_menu.hpp"
#include "core/gettext.hpp"
#include "pushbutton.hpp"
#include "core/event.hpp"
#include "environment.hpp"
#include "core/foreach.hpp"
#include <vector>
#include "layers/constants.hpp"

namespace gui
{

class SubmenuButtons : public std::vector<PushButton*>
{
public:
  void reset()
  {
    for( auto widget : *this )
      widget->deleteLater();

    clear();
  }
};

class OverlaysMenu::Impl
{
public:
  SubmenuButtons buttons;

signals public:
  Signal1<int> onSelectOverlayTypeSignal;
};

OverlaysMenu::OverlaysMenu( Widget* parent, const Rect& rectangle, int id )
  : Widget( parent, id, rectangle ), _d( new Impl )
{
  _addButtons( citylayer::all );
}

void OverlaysMenu::_addButtons(const int type )
{
  Point startPos;
  Point offset( 0, 27 );
  if( Widget* wdg = findChild( type ) )
  {
    startPos = Point( -1 * (int)width() - 10, wdg->top() );
  }

  switch( type )
  {
  case citylayer::all:
    _addButton( citylayer::simple, startPos );
    _addButton( citylayer::water, startPos+=offset );
    _addButton( citylayer::risks, startPos+=offset );
    _addButton( citylayer::entertainments, startPos+=offset );
    _addButton( citylayer::educations, startPos+=offset );
    _addButton( citylayer::healthAll, startPos+=offset );
    _addButton( citylayer::commerce, startPos+=offset );
    _addButton( citylayer::religion, startPos+=offset );
    _addButton( citylayer::products, startPos+=offset );
    setHeight( 9 * offset.y() );
  break;

  case citylayer::risks:
    _addButton( citylayer::fire, startPos );
    _addButton( citylayer::damage, startPos+=offset );
    _addButton( citylayer::crime, startPos+=offset );
    _addButton( citylayer::aborigen, startPos+=offset );
    _addButton( citylayer::troubles, startPos+=offset );
    _addButton( citylayer::sentiment, startPos+=offset );
  break;

  case citylayer::entertainments:
    _addButton( citylayer::entertainment, startPos );
    _addButton( citylayer::theater, startPos+=offset );
    _addButton( citylayer::amphitheater, startPos+=offset );
    _addButton( citylayer::colloseum, startPos+=offset );
    _addButton( citylayer::hippodrome, startPos+=offset );
  break;

  case citylayer::educations:
    _addButton( citylayer::education, startPos );
    _addButton( citylayer::school, startPos+=offset );
    _addButton( citylayer::library, startPos+=offset );
    _addButton( citylayer::academy, startPos+=offset );
  break;

  case citylayer::healthAll:
    _addButton( citylayer::health, startPos );
    _addButton( citylayer::barber, startPos+=offset );
    _addButton( citylayer::baths, startPos+=offset );
    _addButton( citylayer::doctor, startPos+=offset );
    _addButton( citylayer::hospital, startPos+=offset );
  break;

  case citylayer::commerce:
    _addButton( citylayer::tax, startPos );
    _addButton( citylayer::food, startPos+=offset );
    _addButton( citylayer::market, startPos+=offset );
    _addButton( citylayer::desirability, startPos+=offset );
    _addButton( citylayer::unemployed, startPos+=offset );
    _addButton( citylayer::comturnover, startPos+=offset );
  break;

  default: break;
  }
}

void OverlaysMenu::_addButton(const int ovType, const Point& offset )
{
  std::string layerName = citylayer::Helper::prettyName( (citylayer::Type)ovType );
  PushButton& btn = add<PushButton>( Rect( 0, 0, width(), 20 ) + offset, _( layerName ), ovType, false, PushButton::greyBorderLineSmall );
  btn.setFont( "FONT_1" );
  btn.setNotClipped( true );

  if( offset.x() != 0 )
    _d->buttons.push_back( &btn );
}

bool OverlaysMenu::isPointInside( const Point& point ) const
{
  Rect clickedRect = ui()->rootWidget()->absoluteRect();
  return clickedRect.isPointInside( point );
}

bool OverlaysMenu::onEvent( const NEvent& event )
{
  if( event.EventType == sEventGui )
  {
    switch( event.gui.type )
    {
    case guiElementHovered:
      switch( event.gui.caller->ID() )
      {
      case citylayer::risks:
      case citylayer::entertainments:
      case citylayer::educations:
      case citylayer::healthAll:
      case citylayer::commerce:
        {
          _d->buttons.reset();

          _addButtons( event.gui.caller->ID() );
          return true;
        }
      break;

      case citylayer::simple:
      case citylayer::water:
      case citylayer::religion:
        {
          _d->buttons.reset();
          return true;
        }
      break;
      }
    break;

    case guiElementLeft:
    break;

    case guiButtonClicked:
      {
        _d->buttons.reset();

        emit _d->onSelectOverlayTypeSignal( event.gui.caller->ID() );
        hide();
      }
    break;

    default:
    break;
    }
  }

  if( event.EventType == sEventMouse && event.mouse.type == NEvent::Mouse::mouseRbtnRelease )
  {
    hide();
    return true;
  }

  return Widget::onEvent( event );
}

Signal1<int>& OverlaysMenu::onSelectOverlayType() {  return _d->onSelectOverlayTypeSignal; }

}//end namespace gui
