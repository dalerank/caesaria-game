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

#include "overlays_menu.hpp"
#include "core/gettext.hpp"
#include "pushbutton.hpp"
#include "core/event.hpp"
#include "environment.hpp"
#include "core/foreach.hpp"
#include <vector>
#include "gfx/layerconstants.hpp"

using namespace constants;

namespace gui
{

class OverlaysMenu::Impl
{
public:
  typedef std::vector< PushButton* > SubmenuButtons;
  SubmenuButtons buttons;

oc3_signals public:
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
    startPos = Point( -1 * (int)width() - 10, wdg->getTop() );
  }

  switch( type )
  {
  case citylayer::all:
    _addButton( citylayer::simple, _("##ovrm_nothing##"), startPos );
    _addButton( citylayer::water, _("##ovrm_water##"), startPos+=offset );
    _addButton( citylayer::risks, _("##ovrm_risk##"), startPos+=offset );
    _addButton( citylayer::entertainments, _("##ovrm_entertainment##"), startPos+=offset );
    _addButton( citylayer::educations, _("##ovrm_education##"), startPos+=offset );
    _addButton( citylayer::health, _("##ovrm_health##"), startPos+=offset );
    _addButton( citylayer::commerce, _("##ovrm_commerce##"), startPos+=offset );
    _addButton( citylayer::religion, _("##ovrm_religion##"), startPos+=offset );
    setHeight( 8 * offset.y() );
    break;

  case citylayer::risks:
    _addButton( citylayer::fire, _("##ovrm_fire##"), startPos );
    _addButton( citylayer::damage, _("##ovrm_damage##"), startPos+=offset );
    _addButton( citylayer::crime, _("##ovrm_crime##"), startPos+=offset );
    _addButton( citylayer::aborigen, _("##ovrm_aborigen##"), startPos+=offset );
    _addButton( citylayer::troubles, _("##ovrm_troubles##"), startPos+=offset );
    break;

  case citylayer::entertainments:
    _addButton( citylayer::entertainment, _("##ovrm_entr_all##"), startPos );
    _addButton( citylayer::theater, _("##ovrm_theatres##"), startPos+=offset );
    _addButton( citylayer::amphitheater, _("##ovrm_amtheatres##"), startPos+=offset );
    _addButton( citylayer::colloseum, _("##ovrm_colliseum##"), startPos+=offset );
    _addButton( citylayer::hippodrome, _("##ovrm_hpdrome##"), startPos+=offset );
    break;

  case citylayer::educations:
    _addButton( citylayer::education, _("##ovrm_edct_all##"), startPos );
    _addButton( citylayer::school, _("##ovrm_school##"), startPos+=offset );
    _addButton( citylayer::library, _("##ovrm_library##"), startPos+=offset );
    _addButton( citylayer::academy, _("##ovrm_academy##"), startPos+=offset );
    break;

  case citylayer::health:
    _addButton( citylayer::barber, _("##ovrm_barber##"), startPos );
    _addButton( citylayer::baths, _("##ovrm_bath##"), startPos+=offset );
    _addButton( citylayer::doctor, _("##ovrm_clinic##"), startPos+=offset );
    _addButton( citylayer::hospital, _("##ovrm_hospital##"), startPos+=offset );
    break;

  case citylayer::commerce:
    _addButton( citylayer::tax, _("##ovrm_tax##"), startPos );
    _addButton( citylayer::food, _("##ovrm_food##"), startPos+=offset );
    _addButton( citylayer::desirability, _("##ovrm_prestige##"), startPos+=offset );
    break;

  default: break;
  }  
}

void OverlaysMenu::_addButton(const int ovType, const std::string& name, const Point& offset )
{
  PushButton* btn = new PushButton( this, Rect( 0, 0, width(), 20 ) + offset, name, ovType, false, PushButton::smallGrayBorderLine );
  btn->setFont( Font::create( FONT_1 ) );
  btn->setNotClipped( true );
  
  if( offset.x() != 0 )
    _d->buttons.push_back( btn );
}

bool OverlaysMenu::isPointInside( const Point& point ) const
{
  Rect clickedRect = const_cast< OverlaysMenu* >( this )->getEnvironment()->rootWidget()->absoluteRect();
  return clickedRect.isPointInside( point );
}

bool OverlaysMenu::onEvent( const NEvent& event )
{
  if( event.EventType == sEventGui )
  {
    switch( event.gui.type )
    {
    case guiElementHovered:
      switch( event.gui.caller->getID() )
      {
      case citylayer::risks:
      case citylayer::entertainments:
      case citylayer::educations:
      case citylayer::health:
      case citylayer::commerce:
        {
          foreach( item, _d->buttons )  { (*item)->deleteLater(); }

          _d->buttons.clear();

          _addButtons( event.gui.caller->getID() );
          return true;
        }
      break;

      case citylayer::simple:
      case citylayer::water:
      case citylayer::religion:
        {
          foreach( item, _d->buttons )  { (*item)->deleteLater(); }

          _d->buttons.clear();
          return true;
        }
      break;
      }
    break;

    case guiElementLeft:
    break;

    case guiButtonClicked:
      {
        for( Impl::SubmenuButtons::iterator it=_d->buttons.begin(); it != _d->buttons.end(); it++ )
          (*it)->deleteLater();
        _d->buttons.clear();
                
        _d->onSelectOverlayTypeSignal.emit( event.gui.caller->getID() );
        hide();
      }
    break;

    default:
    break;
    }
  } 

  if( event.EventType == sEventMouse && event.mouse.type == mouseRbtnRelease )
  {
    hide();
    return true;
  }

  return Widget::onEvent( event );
}

Signal1<int>& OverlaysMenu::onSelectOverlayType()
{
  return _d->onSelectOverlayTypeSignal;
}

}//end namespace gui
