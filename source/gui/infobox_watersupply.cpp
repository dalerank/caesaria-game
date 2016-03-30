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

#include <cstdio>

#include "infobox_watersupply.hpp"
#include "label.hpp"
#include "objects/fountain.hpp"
#include "core/gettext.hpp"
#include "dictionary.hpp"
#include "objects/well.hpp"
#include "objects/constants.hpp"
#include "core/foreach.hpp"
#include "environment.hpp"
#include "objects/house.hpp"
#include "objects/watersupply.hpp"
#include "game/infoboxmanager.hpp"

using namespace gfx;

namespace gui
{

namespace infobox
{

REGISTER_OBJECT_BASEINFOBOX(well,AboutWell)

AboutWell::AboutWell(Widget* parent, PlayerCityPtr city, const Tile& tile)
  : AboutConstruction( parent, Rect( 0, 0, 480, 320 ), Rect() )
{
  setTitle( _("##well##") );

  _lbText()->setGeometry( Rect( 25, 45, width() - 25, height() - 55 ) );
  _lbText()->setWordwrap( true );

  WellPtr well = tile.overlay<Well>();
  setBase( well );

  std::string text;
  if( well.isValid() )
  {
    TilesArea coverageArea = well->coverageArea();

    bool haveHouseInArea = false;
    for( auto tile : coverageArea )
    {
      haveHouseInArea |= tile->overlay().is<House>();
    }

    if( !haveHouseInArea )
    {
      text = "##well_haveno_houses_inarea##";
    }
    else
    {
      bool houseNeedWell = false;
      auto houses = coverageArea.overlays().select<House>();
      for( auto& house : houses )
      {
        houseNeedWell |= ( house->getServiceValue( Service::fountain ) == 0 );
      }

      if( !houseNeedWell )
      {
        text = "##also_fountain_in_well_area##";
      }
      else
      {
        auto houses = well->coverageArea().overlays<House>();
        int haveLowHealthHouse = houses.count( [] (HousePtr h) { return h->state( pr::health ) < 10; });

        text = haveLowHealthHouse > 0
                ? "##well_infected_info##"
                : "##well_info##";
      }
    }
  }

  _lbText()->setText( _(text) );
}

AboutWell::~AboutWell() {}
void AboutWell::_showHelp() { ui()->add<DictionaryWindow>( object::well ); }

}//end namespace infobox

}//end namespace gui
