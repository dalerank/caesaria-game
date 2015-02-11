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

#include "infobox_land.hpp"
#include "label.hpp"
#include "core/gettext.hpp"
#include "city/city.hpp"
#include "objects/road.hpp"
#include "core/utils.hpp"
#include "objects/constants.hpp"
#include "pathway/pathway_helper.hpp"
#include "dictionary.hpp"

using namespace constants;
using namespace gfx;

namespace gui
{

namespace infobox
{

AboutLand::AboutLand(Widget* parent, PlayerCityPtr city, const Tile& tile )
  : Simple( parent, Rect( 0, 0, 510, 350 ), Rect( 16, 60, 510 - 16, 60 + 180) )
{ 
  Label* lbText = new Label( this, Rect( 38, 60, 470, 60+180 ), "", true, Label::bgNone, lbTextId );
  lbText->setFont( Font::create( FONT_2 ) );
  lbText->setTextAlignment( align::upperLeft, align::center );
  lbText->setWordwrap( true );

  std::string text;
  std::string title;

  if( tile.pos() == city->borderInfo().roadExit )
  {
    title = "##to_empire_road##";
    _helpUri = "road_to_empire";
    text = "";
  }
  else if( tile.pos() == city->borderInfo().boatEntry )
  {
    title = "##to_rome_road##";
    text = "";
  }
  else if( tile.getFlag( Tile::tlTree ) )
  {
    title = "##trees_and_forest_caption##";
    _helpUri = "trees";
    text = "##trees_and_forest_text##";    
  } 
  else if( tile.getFlag( Tile::tlWater ) )
  {
    std::string typeStr = tile.getFlag( Tile::tlCoast )
                            ? "##coast"
                            : "##water";
    title = typeStr + "_caption##";

    TilePos exitPos = city->borderInfo().boatEntry;
    Pathway way = PathwayHelper::create( tile.pos(), exitPos, PathwayHelper::deepWaterFirst );

    text = way.isValid()
             ? (typeStr + "_text##")
             : "##inland_lake_text##";
    _helpUri = "water";
  }
  else if( tile.getFlag( Tile::tlRock ) )
  {
    title = "##rock_caption##";
    _helpUri = "rock";
    text = "##rock_text##";
  }
  else if( tile.getFlag( Tile::tlRoad ) )
  {
    int ovType = tile.overlay().isValid() ? tile.overlay()->type() : objects::unknown;
    if(ovType == objects::plaza )
    {
      title = "##plaza_caption##";
      _helpUri = "plaza";
      text = "##plaza_text##";
    }
    else if( ovType == objects::road )
    {
      _helpUri = "paved_road";
      RoadPtr road = ptr_cast<Road>( tile.overlay() );
      title = road->pavedValue() > 0 ? "##road_paved_caption##" : "##road_caption##";
      if( tile.pos() == city->borderInfo().roadEntry ) { text = "##road_from_rome##"; }
      else if( tile.pos() == city->borderInfo().roadExit ) { text = "##road_to_distant_region##"; }
      else text = road->pavedValue() > 0 ? "##road_paved_text##" : "##road_text##";
    }
    else
    {
      title = "##road_caption##";
      _helpUri = "road";
      text = "##road_unknown_text##";
    }
  }
  else if( tile.getFlag( Tile::tlMeadow ) )
  {
    title = "##meadow_caption##";
    _helpUri = "meadow";
    text = "##meadow_text##";
  }
  else 
  {
    title = "##clear_land_caption##";
    _helpUri = "clear_land";
    text = "##clear_land_text##";
  }
  
  //int index = (size - tile.getJ() - 1 + border_size) * 162 + tile.getI() + border_size;

  text = _(text );
  text += utils::format( 0xff, "\nTile at: (%d,%d) ID:%04X",
                                           tile.i(), tile.j(),  
                                          ((short int) tile.originalImgId() ) );
  
  setTitle( _( title ));
  setText( text );
}

void AboutLand::setText( const std::string& text )
{
  if( Widget* lb = findChild( lbTextId ) )
    lb->setText( text );
}

void AboutLand::_showHelp()
{
  DictionaryWindow::show( this, _helpUri );
}

AboutFreeHouse::AboutFreeHouse( Widget* parent, PlayerCityPtr city, const Tile& tile )
    : AboutLand( parent, city, tile )
{
  setTitle( _("##freehouse_caption##") );

  ConstructionPtr cnst = ptr_cast<Construction>( tile.overlay() );
  if( cnst.isValid() && cnst->getAccessRoads().size() == 0 )
  {
    setText( _("##freehouse_text_noroad##") );
  }
  else
  {
    setText( _("##freehouse_text##") );
    }
}

void AboutFreeHouse::_showHelp()
{
  DictionaryWindow::show( this, "vacant_lot" );
}

}

}//end namespace gui
