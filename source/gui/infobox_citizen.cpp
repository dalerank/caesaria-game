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

#include "infobox_citizen.hpp"
#include "label.hpp"
#include "walker/walker.hpp"
#include "walker/merchant.hpp"
#include "game/settings.hpp"
#include "walker/constants.hpp"
#include "walker/helper.hpp"
#include "core/gettext.hpp"
#include "events/playsound.hpp"

using namespace constants;

namespace gui
{

InfoboxCitizen::InfoboxCitizen(Widget* parent, const WalkerList& walkers )
  : InfoboxSimple( parent, Rect( 0, 0, 460, 350 ), Rect( 18, 40, 460 - 18, 350 - 120 ) )
{
  Widget::setupUI( GameSettings::rcpath( "/gui/infoboxcitizen.gui") );

  new Label( this, Rect( 25, 100, width() - 25, height() - 130), "", false, Label::bgWhiteBorderA );
  new Label( this, Rect( 28, 103, width() - 28, height() - 133), "", false, Label::bgBlack );
  //mini screenshot from citizen pos need here
  new Label( this, Rect( 25, 45, 25 + 52, 45 + 52), "", false, Label::bgBlackFrame );

  Label* lbName = new Label( this, Rect( 90, 108, width() - 30, 108 + 20) );
  lbName->setFont( Font::create( FONT_2 ));
  Label* lbType = new Label( this, Rect( 90, 128, width() - 30, 128 + 20) );
  lbType->setFont( Font::create( FONT_1 ));

  Label* lbThinks = new Label( this, Rect( 90, 148, width() - 30, height() - 140),
                               "##citizen_thoughts_will_be_placed_here##" );
  lbThinks->setWordwrap( true );
  Label* lbCitizenPic = new Label( this, Rect( 30, 112, 30 + 55, 112 + 80) );

  WalkerPtr wlk;
  if( !walkers.empty() )
  {
    wlk = walkers.front();
    lbName->setText( wlk->getName() );

    std::string walkerType = WalkerHelper::getPrettyTypeName( wlk->type() );
    lbType->setText( _(walkerType) );
    lbCitizenPic->setBackgroundPicture( WalkerHelper::getBigPicture( wlk->type() ) );

    std::string thinks = wlk->getThinks();
    lbThinks->setText( _( thinks ) );

    if( !thinks.empty() )
    {
      std::string sound = thinks.substr( 2, thinks.size() - 4 );
      sound += ".wav";
      events::GameEventPtr e = events::PlaySound::create( sound, 100 );
      e->dispatch();
    }
  }

  if( wlk.isValid() )
  {
    switch( wlk->type() )
    {
    case walker::merchant:
    {
      MerchantPtr m = ptr_cast<Merchant>( wlk );
      setTitle( _("##trade_caravan_from##") + m->getParentCity() );
    }
    break;

    default: setTitle( _("##citizen##") );
    }
  }
}

InfoboxCitizen::~InfoboxCitizen()
{
}

}//end namespace gui
