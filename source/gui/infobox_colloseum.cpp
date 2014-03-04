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
#include "objects/entertainment.hpp"
#include "label.hpp"
#include "core/gettext.hpp"

using namespace constants;

namespace gui
{

InfoboxColosseum::InfoboxColosseum(Widget *parent, const Tile &tile)
  : InfoboxConstruction( parent, Rect( 0, 0, 470, 300), Rect( 16, 145, 470 - 16, 145 + 100 ) )
{
  CollosseumPtr colloseum = ptr_cast<Collosseum>(tile.overlay());
  setConstruction( ptr_cast<Construction>( colloseum ) );
  setTitle( _( MetaDataHolder::getPrettyName( building::colloseum ) ) );

  _updateWorkersLabel( Point( 40, 150), 542, colloseum->maxWorkers(), colloseum->numberWorkers() );
  
  if( colloseum->isNeedGladiators() )
  {
    new Label( this, Rect( 35, 190, width() - 35, 190 + 20 ), _("##colloseum_haveno_gladiatorpit##") );
  }
  else
  {
    std::string text = StringHelper::format( 0xff, "Animal contest runs for another %d days", 0 );
    new Label( this, Rect( 35, 200, width() - 35, 200 + 20 ), text );

    text = StringHelper::format( 0xff, "Gladiator bouts runs for another %d days", 0 );
    new Label( this, Rect( 35, 220, width() - 35, 220 + 20 ), text );
  }
}

InfoboxColosseum::~InfoboxColosseum() {}

}//end namespace gui
