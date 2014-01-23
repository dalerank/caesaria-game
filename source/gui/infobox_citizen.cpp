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
#include "core/gettext.hpp"

using namespace constants;

namespace gui
{

InfoBoxCitizen::InfoBoxCitizen(Widget* parent, const WalkerList& walkers )
  : InfoBoxSimple( parent, Rect( 0, 0, 460, 350 ), Rect( 18, 40, 460 - 18, 350 - 120 ) )
{
  Widget::setupUI( GameSettings::rcpath( "/gui/infoboxcitizen.gui") );

  new Label( this, Rect( 25, 100, getWidth() - 25, getHeight() - 130), "", false, Label::bgWhiteBorderA );
  new Label( this, Rect( 28, 103, getWidth() - 28, getHeight() - 133), "", false, Label::bgBlack );
  //mini screenshot from citizen pos need here
  new Label( this, Rect( 25, 45, 25 + 52, 45 + 52), "", false, Label::bgBlackFrame );

  Label* lbName = new Label( this, Rect( 90, 108, getWidth() - 30, 108 + 20) );
  lbName->setFont( Font::create( FONT_2 ));
  Label* lbType = new Label( this, Rect( 90, 128, getWidth() - 30, 128 + 20) );
  lbType->setFont( Font::create( FONT_1 ));

  Label* lbThinks = new Label( this, Rect( 90, 148, getWidth() - 30, getHeight() - 140),
                               "Citizen's thoughts will be placed here" );
  lbThinks->setWordwrap( true );
  Label* lbCitizenPic = new Label( this, Rect( 30, 112, 30 + 55, 112 + 80) );

  WalkerPtr wlk;
  if( !walkers.empty() )
  {
    wlk = walkers.front();
    lbName->setText( wlk->getName() );

    std::string walkerType = WalkerHelper::getPrettyTypeName( wlk->getType() );
    lbType->setText( _(walkerType) );
    lbCitizenPic->setBackgroundPicture( WalkerHelper::getBigPicture( wlk->getType() ) );

    lbThinks->setText( wlk->getThinks() );
  }

  if( wlk.isValid() )
  {
    switch( wlk->getType() )
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

InfoBoxCitizen::~InfoBoxCitizen()
{
}

}//end namespace gui
