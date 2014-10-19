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

#include "infobox_fort.hpp"
#include "objects/fort.hpp"
#include "objects/extension.hpp"
#include "core/gettext.hpp"
#include "label.hpp"

using namespace constants;
using namespace gfx;

namespace gui
{

namespace infobox
{

AboutFort::AboutFort( Widget* parent, const Tile& tile )
  : Simple( parent, Rect( 0, 0, 510, 350 ) )
{
  FortPtr fort = ptr_cast<Fort>( tile.overlay() );
  setTitle( MetaDataHolder::findPrettyName( fort->type() ) );

  std::string text = _("##fort_info##");

  if( fort.isValid() )
  {
    const ConstructionExtensionList& exts = fort->extensions();
    foreach( i, exts )
    {
      if( is_kind_of<FortCurseByMars>( *i ) )
      {
        text = "##fort_has_been_cursed_by_mars##";
        break;
      }
    }
  }

  Label* lb = new Label( this, Rect( 20, 20, width() - 20, height() - 50), text );
  lb->setTextAlignment( align::upperLeft, align::center );
  lb->setWordwrap( true );
}

AboutFort::~AboutFort()
{
}

}

}//end namespace gui
