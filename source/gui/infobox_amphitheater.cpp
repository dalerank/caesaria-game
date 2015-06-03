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

#include "infobox_amphitheater.hpp"
#include "gfx/tile.hpp"
#include "objects/constants.hpp"
#include "objects/amphitheater.hpp"
#include "label.hpp"
#include "core/gettext.hpp"
#include "game/gamedate.hpp"
#include "core/utils.hpp"
#include "game/infoboxmanager.hpp"

using namespace gfx;

namespace gui
{

namespace infobox
{

REGISTER_OBJECT_BASEINFOBOX(amphitheater,AboutAmphitheater)

AboutAmphitheater::AboutAmphitheater(Widget *parent, PlayerCityPtr city, const Tile &tile)
  : AboutConstruction( parent, Rect( 0, 0, 470, 300), Rect( 16, 145, 470 - 16, 145 + 100 ) )
{
  setupUI( ":/gui/infoboxapmhitheater.gui" );

  AmphitheaterPtr amph = tile.overlay().as<Amphitheater>();
  setBase( amph );
  setTitle( _( MetaDataHolder::findPrettyName( object::amphitheater ) ) );
  _setWorkingVisible( true );

  _updateWorkersLabel( Point( 40, 150), 542, amph->maximumWorkers(), amph->numberWorkers() );

  if( amph->isNeed( walker::gladiator ) )
    _updateWarnings();
  else
  {
    _updateBouthsInfo();
    _updateShowsInfo();
  }

}

AboutAmphitheater::~AboutAmphitheater() {}

void AboutAmphitheater::_updateWarnings()
{
  new Label( this, Rect( 35, 190, width() - 35, 190 + 20 ), _("##amphitheater_haveno_gladiatorpit##") );
}

void AboutAmphitheater::_updateShowsInfo()
{
  std::string text = _("##amphitheater_haveno_shows##");
  if( amph->isShow( Amphitheater::theatrical ) )
  {
    DateTime lastShowDate = amph->lastShow( Amphitheater::theatrical );
    text = utils::format( 0xff, "%s %d %s", "##amphitheater_show_runs##", lastShowDate.daysTo( game::Date::current() ), "##days##" );
  }

  new Label( this, Rect( 35, 220, width() - 35, 220 + 20 ), text );
}

void AboutAmphitheater::_updateBouthsInfo()
{
  std::string text = _("##amphitheater_haveno_gladiator_bouts##");
  if( amph->isShow( Amphitheater::gladiatorBouts ) )
  {
    DateTime lastGlBoutDate = amph->lastShow( Amphitheater::gladiatorBouts );
    text = utils::format( 0xff, "%s %d %s", "##amphitheater_gladiator_contest_runs##", lastGlBoutDate.daysTo( game::Date::current() ), "##days##" );
  }
  new Label( this, Rect( 35, 200, width() - 35, 200 + 20 ), text );
}

}

}//end namespace gui
