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
#include "objects/well.hpp"
#include "objects/constants.hpp"
#include "core/foreach.hpp"
#include "objects/house.hpp"

using namespace constants;

namespace gui
{

InfoBoxFontain::InfoBoxFontain(Widget* parent, const Tile& tile)
  : InfoBoxSimple( parent, Rect( 0, 0, 480, 320 ), Rect( 0, 0, 1, 1 ) )
{
  setTitle( _("##fountain##") );

  _getInfo()->setGeometry( Rect( 25, 45, getWidth() - 25, getHeight() - 55 ) );
  _getInfo()->setWordwrap( true );

  FountainPtr fountain = ptr_cast<Fountain>( tile.getOverlay() );
  std::string text;
  if( fountain != 0 )
  {
    if( fountain->isActive() )
    {
      text = "##fountain_info##";
    }
    else
    {
      text = fountain->haveReservoirAccess()
               ? "##need_full_reservoir_for_work##"
               : "##need_reservoir_for_work##";
    }
  }

  _getInfo()->setText( _(text) );
}

InfoBoxFontain::~InfoBoxFontain()
{

}

void InfoBoxFontain::showDescription()
{
  DictionaryWindow::show( getParent(), building::fountain );
}

InfoboxWell::InfoboxWell(Widget* parent, const Tile& tile)
  : InfoBoxSimple( parent, Rect( 0, 0, 480, 320 ), Rect() )
{
  setTitle( "##well##" );

  _getInfo()->setGeometry( Rect( 25, 45, getWidth() - 25, getHeight() - 55 ) );
  _getInfo()->setWordwrap( true );

  WellPtr well = ptr_cast<Well>( tile.getOverlay() );
  std::string text;
  if( well.isValid() )
  {
    TilesArray coverageArea = well->getCoverageArea();

    bool haveHouseInArea = false;
    foreach( tile, coverageArea )
    {
      haveHouseInArea |= is_kind_of<House>( (*tile)->getOverlay() );
    }

    if( !haveHouseInArea )
    {
      text = "##well_haveno_houses_inarea##";
    }
    else
    {
      bool houseNeedWell = false;
      foreach( tile, coverageArea)
      {
        HousePtr house = ptr_cast<House>( (*tile)->getOverlay() );
        if( house.isValid() )
        {
          houseNeedWell |= ( house->getServiceValue( Service::fontain ) == 0 );
        }
      }

      if( !houseNeedWell )
      {
        text = "##also_fountain_in_well_area##";
      }
      else
      {
        text = "##well_info##";
      }
    }
  }

  _getInfo()->setText( _(text) );
}

InfoboxWell::~InfoboxWell()
{

}

void InfoboxWell::showDescription()
{
  DictionaryWindow::show( getParent(), building::well );
}


}//end namespace gui
