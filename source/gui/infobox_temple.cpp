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

#include "infobox_temple.hpp"
#include "objects/religion.hpp"
#include "religion/romedivinity.hpp"
#include "core/gettext.hpp"
#include "religion/romedivinity.hpp"
#include "image.hpp"
#include "core/utils.hpp"
#include "core/format.hpp"
#include "game/infoboxmanager.hpp"
#include "events/script_event.hpp"

using namespace religion;
using namespace gfx;

namespace gui
{

namespace infobox
{

REGISTER_OBJECT_FREEINFOBOX(small_ceres_temple,AboutTemple)
REGISTER_OBJECT_FREEINFOBOX(small_mars_temple,AboutTemple)
REGISTER_OBJECT_FREEINFOBOX(small_neptune_temple,AboutTemple)
REGISTER_OBJECT_FREEINFOBOX(small_venus_temple,AboutTemple)
REGISTER_OBJECT_FREEINFOBOX(small_mercury_temple,AboutTemple)
REGISTER_OBJECT_FREEINFOBOX(big_ceres_temple,AboutTemple)
REGISTER_OBJECT_FREEINFOBOX(big_mars_temple,AboutTemple)
REGISTER_OBJECT_FREEINFOBOX(big_neptune_temple,AboutTemple)
REGISTER_OBJECT_FREEINFOBOX(big_venus_temple,AboutTemple)
REGISTER_OBJECT_FREEINFOBOX(big_mercury_temple,AboutTemple)
REGISTER_OBJECT_FREEINFOBOX(oracle,AboutTemple)

AboutTemple::AboutTemple(Widget* parent, PlayerCityPtr city, const Tile& tile )
{
  TemplePtr temple = tile.overlay<Temple>();
  //setBase( temple );

  std::string shortDesc;
  std::string text, longDescr;
  /*if( temple.is<TempleOracle>() )
  {
     text = _( "##oracle##" );
     setText( _( "##oracle_info##" ) );
  }
  else
  {
    DivinityPtr divn = temple->divinity();
     bool bigTemple = temple->size().width() > 2;
     shortDesc =  _( divn->shortDescription() );
     text = _( fmt::format( "##{}_{}_temple##",
                              bigTemple ? "big" : "small",
                              divn->debugName() ) );

     bool goodRelation = divn->relation() >= 50;

     longDescr = fmt::format( "##{}_{}_info##",
                              divn->internalName(),
                              goodRelation ? "goodmood" : "badmood" );

     Image& img = add<Image>( Point( 192, 140 ), divn->picture() );
     img.setTooltipText( _(longDescr) );
  }

  if( !shortDesc.empty() )
    shortDesc = " ( " + shortDesc + " )";

  setTitle( text + shortDesc );

  _updateWorkersLabel( Point( 32, 56 + 12), 542, temple->maximumWorkers(), temple->numberWorkers() );  */

  VariantList vl; vl << tile.epos();
  events::dispatch<events::ScriptFunc>("OnShowTempleInfobox", vl);
}

}//end namespace infobox

}//end namespace gui
