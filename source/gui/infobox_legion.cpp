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
#include "pushbutton.hpp"
#include "game/resourcegroup.hpp"

using namespace constants;

namespace gui
{

InfoboxLegion::InfoboxLegion(Widget* parent, PatrolPointPtr patrolPoint )
  : InfoboxSimple( parent, Rect( 0, 0, 460, 350 ), Rect() )
{
  Widget::setupUI( GameSettings::rcpath( "/gui/legionopts.gui") );

  Point offset( 83, 0 );
  Rect rect( Point( 100, 140 ), Size( 83 ) );
  PushButton* btn = new PushButton( this, rect );
  btn->setBackgroundStyle( PushButton::whiteBorderUp );
  btn->setIcon( ResourceGroup::menuMiddleIcons, 28 );
  btn->setIconOffset( Point( 1, 1 ) );

  rect += offset;
  btn = new PushButton( this, rect );
  btn->setBackgroundStyle( PushButton::whiteBorderUp );
  btn->setIcon( ResourceGroup::menuMiddleIcons, 30 );
  btn->setIconOffset( Point( 1, 1 ) );

  rect += offset;
  btn = new PushButton( this, rect );
  btn->setBackgroundStyle( PushButton::whiteBorderUp );
  btn->setIcon( ResourceGroup::menuMiddleIcons, 31 );
  btn->setIconOffset( Point( 1, 1 ) );

  rect += offset;
  btn = new PushButton( this, rect );
  btn->setBackgroundStyle( PushButton::whiteBorderUp );
  btn->setIcon( ResourceGroup::menuMiddleIcons, 32 );
  btn->setIconOffset( Point( 1, 1 ) );
}

InfoboxLegion::~InfoboxLegion()
{
}

}//end namespace gui
