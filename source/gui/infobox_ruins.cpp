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

#include "infobox_ruins.hpp"
#include "good/goodhelper.hpp"
#include "image.hpp"
#include "core/stringhelper.hpp"
#include "label.hpp"
#include "core/gettext.hpp"
#include "objects/constants.hpp"
#include "game/settings.hpp"
#include "objects/ruins.hpp"

using namespace constants;

namespace gui
{

InfoboxRuins::InfoboxRuins( Widget* parent, const Tile& tile )
  : InfoboxSimple( parent, Rect( 0, 0, 510, 350 ) )
{
  RuinsPtr ruin = ptr_cast<Ruins>( tile.getOverlay() );
  std::string text = _("##ruins_0000_text##");
  if( ruin.isValid() )
  {
    if( !ruin->getInfo().empty() )
    {
      text = _( ruin->getInfo().c_str() );
    }
  }
  Label* lb = new Label( this, Rect( 20, 20, getWidth() - 20, getHeight() - 50), text );
  lb->setTextAlignment( alignCenter, alignCenter );
  lb->setWordwrap( true );
}

InfoboxRuins::~InfoboxRuins()
{
}

}//end namespace gui
