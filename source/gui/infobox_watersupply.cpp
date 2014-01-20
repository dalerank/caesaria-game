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

#include "infobox_watersupply.hpp"
#include "label.hpp"
#include "objects/watersupply.hpp"
#include "core/gettext.hpp"
#include "dictionary.hpp"
#include "objects/constants.hpp"

using namespace constants;

namespace gui
{

InfoBoxFontain::InfoBoxFontain(Widget* parent, const Tile& tile)
  : InfoBoxSimple( parent, Rect( 0, 0, 480, 320 ), Rect( 0, 0, 1, 1 ) )
{
  setTitle( "##fountain##" );

  _getInfo()->setGeometry( Rect( 25, 45, getWidth() - 25, getHeight() - 55 ) );
  _getInfo()->setWordwrap( true );

  FountainPtr fountain = tile.getOverlay().as<Fountain>();
  std::string text;
  if( fountain != 0 )
  {
    if( fountain->isActive() )
    {
      text = _("##fountain_info##");
    }
    else
    {
      text = fountain->haveReservoirAccess()
               ? _("##need_full_reservoir_for_work##")
               : _("##need_reservoir_for_work##");
    }
  }

  _getInfo()->setText( text );
}

InfoBoxFontain::~InfoBoxFontain()
{

}

void InfoBoxFontain::showDescription()
{
  DictionaryWindow::show( getParent(), building::fountain );
}

}//end namespace gui
