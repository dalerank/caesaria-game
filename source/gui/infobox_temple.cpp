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

using namespace constants;
using namespace religion;
using namespace gfx;

namespace gui
{

namespace infobox
{

AboutTemple::AboutTemple(Widget* parent, PlayerCityPtr city, const Tile& tile )
  : AboutConstruction( parent, Rect( 0, 0, 510, 256 ), Rect( 16, 56, 510 - 16, 56 + 62) )
{
  setupUI( ":/gui/infoboxtemple.gui" );

  TemplePtr temple = ptr_cast<Temple>( tile.overlay() );
  DivinityPtr divn = temple->divinity();

  setBase( ptr_cast<Construction>( temple ) );

  bool bigTemple = temple->size().width() > 2;
  std::string desc = _( divn->shortDescription() );
  std::string text = _( utils::format( 0xff, "##%s_%s_temple##",
                                             bigTemple ? "big" : "small",
                                             divn->debugName().c_str() ) );
  setTitle( text + " ( " + desc + " )" );

  _updateWorkersLabel( Point( 32, 56 + 12), 542, temple->maximumWorkers(), temple->numberWorkers() );

  Image* img = new Image( this, Point( 192, 140 ), divn->picture() );
  bool goodRelation = divn->relation() >= 50;

  std::string descr = utils::format(0xff, "##%s_%s_info##",
                                          divn->internalName().c_str(),
                                          goodRelation ? "goodmood" : "badmood" );
  img->setTooltipText( _(descr) );
}

AboutTemple::~AboutTemple()
{
}

}

}//end namespace gui
