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

#include <cstdio>

#include "walker/patrolpoint.hpp"
#include "infobox_legion.hpp"
#include "environment.hpp"
#include "core/foreach.hpp"
#include "game/settings.hpp"
#include "label.hpp"
#include "core/gettext.hpp"
#include "pushbutton.hpp"
#include "game/resourcegroup.hpp"
#include "core/event.hpp"

using namespace constants;

namespace gui
{

class InfoboxLegion::Impl
{
public:
  Label* lbFormationTitle;
  Label* lbFormationText;
};

InfoboxLegion::InfoboxLegion(Widget* parent, WalkerList walkers )
  : InfoboxSimple( parent, Rect( 0, 0, 460, 350 ), Rect() ), _d( new Impl )
{  
  Widget::setupUI( GameSettings::rcpath( "/gui/legionopts.gui") );

  _d->lbFormationTitle = findChildA<Label*>( "lbFormationTitle", true, this );
  _d->lbFormationText = findChildA<Label*>( "lbFormation", true, this );

  _addFormationButton( 0, 28 );
  _addFormationButton( 1, 30 );
  _addFormationButton( 2, 31 );
  _addFormationButton( 3, 32 );
}

InfoboxLegion::~InfoboxLegion() {}

bool InfoboxLegion::onEvent(const NEvent& event)
{
  if( event.EventType == sEventGui && event.gui.type == guiButtonClicked )
  {
    std::string text;
    switch( event.gui.caller->getID() )
    {
    case 28: text = "##defensive_formation"; break;
    case 30: text = ""; break;
    case 31: text = "##simple_formation"; break;
    case 32: text = ""; break;

    default:
    break;
    }

    if( !text.empty() && _d->lbFormationTitle && _d->lbFormationText )
    {
      _d->lbFormationText->setText( _(text + "_title##") );
      _d->lbFormationTitle->setText( _(text + "_text##") );
    }
  }

  return InfoboxSimple::onEvent( event );
}

void InfoboxLegion::_addFormationButton(int index, int picId)
{
  Point offset( 83, 0 );
  Rect rect( Point( 100, 140 ) + offset * index, Size( 83 ) );
  PushButton* btn = new PushButton( this, rect, "", picId );
  btn->setBackgroundStyle( PushButton::whiteBorderUp );
  btn->setIcon( ResourceGroup::menuMiddleIcons, picId );
  btn->setIconOffset( Point( 1, 1 ) );
  btn->setTooltipText( _("##legion_formation_tooltip##") );
}

}//end namespace gui
