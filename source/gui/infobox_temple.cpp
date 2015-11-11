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
#include "game/infoboxmanager.hpp"

using namespace religion;
using namespace gfx;

namespace gui
{

namespace infobox
{

REGISTER_OBJECT_BASEINFOBOX(small_ceres_temple,AboutTemple)
REGISTER_OBJECT_BASEINFOBOX(small_mars_temple,AboutTemple)
REGISTER_OBJECT_BASEINFOBOX(small_neptune_temple,AboutTemple)
REGISTER_OBJECT_BASEINFOBOX(small_venus_temple,AboutTemple)
REGISTER_OBJECT_BASEINFOBOX(small_mercury_temple,AboutTemple)
REGISTER_OBJECT_BASEINFOBOX(big_ceres_temple,AboutTemple)
REGISTER_OBJECT_BASEINFOBOX(big_mars_temple,AboutTemple)
REGISTER_OBJECT_BASEINFOBOX(big_neptune_temple,AboutTemple)
REGISTER_OBJECT_BASEINFOBOX(big_venus_temple,AboutTemple)
REGISTER_OBJECT_BASEINFOBOX(big_mercury_temple,AboutTemple)
REGISTER_OBJECT_BASEINFOBOX(oracle,AboutTemple)

AboutTemple::AboutTemple(Widget* parent, PlayerCityPtr city, const Tile& tile )
  : AboutConstruction( parent, Rect( 0, 0, 510, 256 ), Rect( 16, 56, 510 - 16, 56 + 62) )
{
  setupUI( ":/gui/infoboxtemple.gui" );

  TemplePtr temple = tile.overlay<Temple>();
  setBase( temple );

  std::string shortDesc;
  std::string text, longDescr;
  if( temple.is<TempleOracle>() )
  {
     text = _( "##oracle##" );
     setText( _( "##oracle_info##" ) );
  }
  else
  {
     DivinityPtr divn = temple->divinity();
     bool bigTemple = temple->size().width() > 2;
     shortDesc =  _( divn->shortDescription() );
     text = _( utils::format( 0xff, "##%s_%s_temple##",
                              bigTemple ? "big" : "small",
                              divn->debugName().c_str() ) );

     bool goodRelation = divn->relation() >= 50;

     longDescr = utils::format(0xff, "##%s_%s_info##",
                                             divn->internalName().c_str(),
                                             goodRelation ? "goodmood" : "badmood" );

     Image& img = add<Image>( Point( 192, 140 ), divn->picture() );
     img.setTooltipText( _(longDescr) );
  }

  if( !shortDesc.empty() )
    shortDesc = " ( " + shortDesc + " )";

  setTitle( text + shortDesc );

  _updateWorkersLabel( Point( 32, 56 + 12), 542, temple->maximumWorkers(), temple->numberWorkers() );  
}

AboutTemple::~AboutTemple() {}

}

}//end namespace gui
