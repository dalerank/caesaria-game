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

#include "infobox_land.hpp"
#include "label.hpp"
#include "core/gettext.hpp"
#include "objects/constants.hpp"

using namespace constants;

namespace gui
{

InfoboxLand::InfoboxLand( Widget* parent, const Tile& tile )
  : InfoboxSimple( parent, Rect( 0, 0, 510, 350 ), Rect( 16, 60, 510 - 16, 60 + 180) )
{ 
  Label* lbText = new Label( this, Rect( 38, 239, 470, 338 ), "", true, Label::bgNone, lbTextId );
  lbText->setFont( Font::create( FONT_2 ) );
  lbText->setWordwrap( true );

  if( tile.getFlag( Tile::tlTree ) )
  {
    setTitle( _("##trees_and_forest_caption##") );
    lbText->setText( _("##trees_and_forest_text##"));
  } 
  else if( tile.getFlag( Tile::tlWater ) )
  {
    setTitle( _("##water_caption##") );
    lbText->setText( _("##water_text##"));
  }
  else if( tile.getFlag( Tile::tlRock ) )
  {
    setTitle( _("##rock_caption##") );
    lbText->setText( _("##rock_text##"));
  }
  else if( tile.getFlag( Tile::tlRoad ) )
  {
    if( tile.getOverlay()->getType() == construction::plaza )
    {
      setTitle( _("##plaza_caption##") );
      lbText->setText( _("##plaza_text##"));
    }
    else 
    {
      setTitle( _("##road_caption##") );
      lbText->setText( _("##road_text##"));
    }
  }
  else if( tile.getFlag( Tile::tlMeadow ) )
  {
    setTitle( _("##meadow_caption##") );
    lbText->setText( _("##meadow_text##"));
  }
  else 
  {
    setTitle( _("##clear_land_caption##") );
    lbText->setText( _("##clear_land_text##"));
  }
  
  //int index = (size - tile.getJ() - 1 + border_size) * 162 + tile.getI() + border_size;

  std::string text = StringHelper::format( 0xff, "Tile at: (%d,%d) ID:%04X",
                                           tile.getI(), tile.getJ(),  
                                          ((short int) tile.getOriginalImgId() ) );
  
  lbText->setText( text );
}

void InfoboxLand::setText( const std::string& text )
{
  if( Widget* lb = findChild( lbTextId ) )
    lb->setText( text );
}

InfoBoxFreeHouse::InfoBoxFreeHouse( Widget* parent, const Tile& tile )
    : InfoboxLand( parent, tile )
{
  setTitle( _("##freehouse_caption##") );

  ConstructionPtr cnst = ptr_cast<Construction>( tile.getOverlay() );
  if( cnst.isValid() && cnst->getAccessRoads().size() == 0 )
  {
    setText( _("##freehouse_text_noroad##") );
  }
  else
  {
    setText( _("##freehouse_text##") );
  }
}

}//end namespace gui
