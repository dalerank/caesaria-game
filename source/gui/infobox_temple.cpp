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

using namespace constants;
using namespace religion;

namespace gui
{

InfoBoxTemple::InfoBoxTemple( Widget* parent, const Tile& tile )
  : InfoboxConstruction( parent, Rect( 0, 0, 510, 256 ), Rect( 16, 56, 510 - 16, 56 + 62) )
{
  TemplePtr temple = ptr_cast<Temple>( tile.overlay() );
  RomeDivinityPtr divn = temple->getDivinity();

  setConstruction( ptr_cast<Construction>( temple ) );

  bool bigTemple = temple->getSize().width() > 2;
  std::string desc = _( divn->getShortDescription() );
  std::string text = _( StringHelper::format( 0xff, "##%s_%s_temple##",
                                                 bigTemple ? "big" : "small",
                                                 divn->getDebugName().c_str() ) );
  setTitle( text + " ( " + desc + " )" );

  _updateWorkersLabel( Point( 32, 56 + 12), 542, temple->maxWorkers(), temple->numberWorkers() );

  Image* img = new Image( this, Point( 192, 140 ), divn->getPicture() );
  bool goodRelation = divn->getRelation() >= 50;

  std::string descr = StringHelper::format(0xff, "##%s_%s_info##",
                                                  divn->getDebugName().c_str(),
                                                  goodRelation ? "goodmood" : "badmood" );
  img->setTooltipText( _(descr) );
}

InfoBoxTemple::~InfoBoxTemple()
{
}

}//end namespace gui
