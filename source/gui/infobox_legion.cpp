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
#include "walker/romesoldier.hpp"
#include "city/helper.hpp"
#include "game/resourcegroup.hpp"
#include "core/event.hpp"
#include "objects/fort.hpp"
#include "core/logger.hpp"

using namespace constants;

namespace gui
{

class InfoboxLegion::Impl
{
public:
  Label* lbFormationTitle;
  Label* lbFormationText;  
  PushButton* btnReturn;
  FortPtr fort;

oc3_slots public:
  void returnSoldiers2fort();
};

InfoboxLegion::InfoboxLegion(Widget* parent, PlayerCityPtr city, const TilePos& pos  )
  : InfoboxSimple( parent, Rect( 0, 0, 460, 350 ), Rect() ), _d( new Impl )
{  
  Widget::setupUI( GameSettings::rcpath( "/gui/legionopts.gui") );

  _d->lbFormationTitle = findChildA<Label*>( "lbFormationTitle", true, this );
  _d->lbFormationText = findChildA<Label*>( "lbFormation", true, this );
  _d->btnReturn = findChildA<PushButton*>( "btnReturn", true, this );

  _addFormationButton( Fort::frmNorthLine, 33 );
  _addFormationButton( Fort::frmSouthLine, 34 );
  _addFormationButton( Fort::frmNorthDblLine, 30 );
  _addFormationButton( Fort::frmSouthDblLine, 31 );
  _addFormationButton( Fort::frmRandomLocation, 32 );

  WalkerList walkers = city->getWalkers( walker::any, pos );
  foreach( i, walkers )
  {
    RomeSoldierPtr rs = ptr_cast<RomeSoldier>( *i);
    if( rs.isValid() )
    {
      _d->fort = rs->base();
      break;
    }

    PatrolPointPtr pp = ptr_cast<PatrolPoint>( *i);
    if( pp.isValid() )
    {
      _d->fort = pp->base();
      break;
    }
  }

  setTitle( _d->fort.isValid()
                ? _d->fort->legionName()
                : "##unknown_legion##" );

  CONNECT( _d->btnReturn, onClicked(), _d.data(), Impl::returnSoldiers2fort );
}

InfoboxLegion::~InfoboxLegion() {}

bool InfoboxLegion::onEvent(const NEvent& event)
{
  if( event.EventType == sEventGui && event.gui.type == guiButtonClicked )
  {
    int id = event.gui.caller->getID();
    if( id >= 0 )
    {
      std::string text;
      switch( id )
      {
      case Fort::frmNorthLine: text = "##defensive_formation"; break;
      case Fort::frmSouthLine: text = "##defensive_formation2_"; break;
      case Fort::frmNorthDblLine: text = "##simple_formation"; break;
      case Fort::frmSouthDblLine: text = "##simple_formation2"; break;
      case Fort::frmRandomLocation: text = "##simple_random_location"; break;

      default:
      break;
      }

      if( !text.empty() && _d->lbFormationTitle && _d->lbFormationText )
      {
        _d->lbFormationTitle->setText( _(text + "_title##") );
        _d->lbFormationText->setText( _(text + "_text##") );
      }

      if( _d->fort.isValid() )
      {
        _d->fort->setFormation( (Fort::TroopsFormation)id );
      }
    }
  }

  return InfoboxSimple::onEvent( event );
}

void InfoboxLegion::_addFormationButton(int index, int picId)
{
  Point offset( 83, 0 );
  Rect rect( Point( 16, 140 ) + offset * index, Size( 83 ) );
  PushButton* btn = new PushButton( this, rect, "", index );
  btn->setBackgroundStyle( PushButton::whiteBorderUp );
  btn->setIcon( ResourceGroup::menuMiddleIcons, picId );
  btn->setIconOffset( Point( 1, 1 ) );
  btn->setTooltipText( _("##legion_formation_tooltip##") );
}

void InfoboxLegion::Impl::returnSoldiers2fort()
{
  if( fort.isNull() )
      return;

  RomeSoldierList soldiers;
  soldiers << fort->soldiers();
  foreach( i, soldiers )
  {
    (*i)->return2fort();
  }
}

}//end namespace gui
