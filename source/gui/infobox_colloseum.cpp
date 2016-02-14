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

#include "infobox_colloseum.hpp"
#include "gfx/tile.hpp"
#include "objects/constants.hpp"
#include "objects/colosseum.hpp"
#include "label.hpp"
#include "core/gettext.hpp"
#include "game/gamedate.hpp"
#include "core/format.hpp"
#include "game/infoboxmanager.hpp"

using namespace gfx;

namespace gui
{

namespace infobox
{

REGISTER_OBJECT_BASEINFOBOX(colloseum,AboutColosseum)

AboutColosseum::AboutColosseum(Widget *parent, PlayerCityPtr city, const Tile &tile)
  : AboutConstruction( parent, Rect( 0, 0, 470, 300), Rect( 16, 145, 470 - 16, 145 + 100 ) )
{
  setupUI( ":/gui/infoboxcolosseum.gui" );

  auto coloseum = tile.overlay<Colosseum>();

  if( coloseum.isNull() )
  {
    deleteLater();
    return;
  }

  setBase( coloseum );
  _setWorkingVisible( true );
  setTitle( _( coloseum->info().prettyName() ) );

  _updateWorkersLabel( Point( 40, 150), 542, coloseum->maximumWorkers(), coloseum->numberWorkers() );
  
  if( coloseum->isNeedGladiators() )
  {
    add<Label>( Rect( 35, 190, width() - 35, 190 + 20 ), _("##colloseum_haveno_gladiatorpit##") );
  }
  else
  {
    std::string text = _("##colloseum_haveno_animal_bouts##");
    if( coloseum->isShowLionBattles() )
    {
      DateTime lastAnimalBoutDate = coloseum->lastAnimalBoutDate();
      text = fmt::format( "{} {} {}", _("##colloseum_animal_contest_runs##"), lastAnimalBoutDate.daysTo( game::Date::current() ), _("##days##") );
    }
    add<Label>( Rect( 35, 200, width() - 35, 200 + 20 ), text );

    text = _("##colloseum_haveno_gladiator_bouts##");
    if( coloseum->isShowGladiatorBattles() )
    {
      DateTime lastGlBoutDate = coloseum->lastGladiatorBoutDate();
      text = fmt::format( "{} {} {}", _("##colloseum_gladiator_contest_runs##"), lastGlBoutDate.daysTo( game::Date::current() ), _("##days##") );
    }

    add<Label>( Rect( 35, 220, width() - 35, 220 + 20 ), text );
  }
}

AboutColosseum::~AboutColosseum() {}

}

}//end namespace gui
