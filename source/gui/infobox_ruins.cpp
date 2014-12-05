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
//
// Copyright 2012-2014 Dalerank, dalerankn8@gmail.com

#include "infobox_ruins.hpp"
#include "good/goodhelper.hpp"
#include "image.hpp"
#include "core/utils.hpp"
#include "label.hpp"
#include "dictionary.hpp"
#include "core/logger.hpp"
#include "core/gettext.hpp"
#include "objects/constants.hpp"
#include "game/settings.hpp"
#include "objects/ruins.hpp"

using namespace constants;
using namespace gfx;

namespace gui
{

namespace infobox
{

AboutRuins::AboutRuins( Widget* parent, PlayerCityPtr city, const Tile& tile )
  : Simple( parent, Rect( 0, 0, 510, 350 ) )
{
  RuinsPtr ruin = ptr_cast<Ruins>( tile.overlay() );
  if( ruin.isNull() )
  {
    Logger::warning( "AbourRuins: tile overlay at [%d,%d] not ruin", tile.i(), tile.j() );
    return;
  }

  setTitle( MetaDataHolder::findPrettyName( ruin->type() ) );
  std::string text = _("##ruins_0000_text##");
  _ruinType = ruin->type();

  if( ruin.isValid() )
  {
    if( !ruin->info().empty() )
    {
      text = _( ruin->info().c_str() );
    }
  }

  Label* lb = new Label( this, Rect( 20, 20, width() - 20, height() - 50), text );
  lb->setTextAlignment( align::center, align::center );
  lb->setWordwrap( true );
}

AboutRuins::~AboutRuins()
{
}

void AboutRuins::_showHelp()
{
  std::string helpValue = MetaDataHolder::findTypename( _ruinType );

  DictionaryWindow::show( this, helpValue );
}

}

}//end namespace gui
