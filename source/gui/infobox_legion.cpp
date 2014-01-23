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

#include <cstdio>

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

InfoboxLegion::InfoboxLegion(Widget* parent, PatrolPointPtr patrolPoint )
  : InfoboxSimple( parent, Rect( 0, 0, 460, 350 ), Rect() ), _d( new Impl )
{
  Widget::setupUI( GameSettings::rcpath( "/gui/legionopts.gui") );

  _d->lbFormationTitle = findChild<Label*>( "lbFormationTitle", true );
  _d->lbFormationText = findChild<Label*>( "lbFormation", true );

  Point offset( 83, 0 );
  Rect rect( Point( 100, 140 ), Size( 83 ) );
  PushButton* btn = new PushButton( this, rect, "", 28 );
  btn->setBackgroundStyle( PushButton::whiteBorderUp );
  btn->setIcon( ResourceGroup::menuMiddleIcons, 28 );
  btn->setIconOffset( Point( 1, 1 ) );

  rect += offset;
  btn = new PushButton( this, rect, "", 30 );
  btn->setBackgroundStyle( PushButton::whiteBorderUp );
  btn->setIcon( ResourceGroup::menuMiddleIcons, 30 );
  btn->setIconOffset( Point( 1, 1 ) );

  rect += offset;
  btn = new PushButton( this, rect, "", 31 );
  btn->setBackgroundStyle( PushButton::whiteBorderUp );
  btn->setIcon( ResourceGroup::menuMiddleIcons, 31 );
  btn->setIconOffset( Point( 1, 1 ) );

  rect += offset;
  btn = new PushButton( this, rect, "", 32 );
  btn->setBackgroundStyle( PushButton::whiteBorderUp );
  btn->setIcon( ResourceGroup::menuMiddleIcons, 32 );
  btn->setIconOffset( Point( 1, 1 ) );
}

InfoboxLegion::~InfoboxLegion()
{
}

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

}//end namespace gui
