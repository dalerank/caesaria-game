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
#include "objects/house.hpp"
#include "objects/watersupply.hpp"

using namespace constants;
using namespace gfx;

namespace gui
{

namespace infobox
{

AboutFontain::AboutFontain(Widget* parent, PlayerCityPtr city, const Tile& tile)
  : AboutConstruction( parent, Rect( 0, 0, 480, 320 ), Rect( 0, 0, 1, 1 ) )
{
  setupUI( ":/gui/infoboxfountain.gui" );
  setTitle( _("##fountain##") );

  _lbTextRef()->setGeometry( Rect( 25, 45, width() - 25, height() - 55 ) );
  _lbTextRef()->setWordwrap( true );

  FountainPtr fountain;
  fountain << tile.overlay();

  setBase( ptr_cast<Construction>( fountain ) );

  std::string text;
  if( fountain.isValid() )
  {
    if( fountain->haveReservoirAccess() && tile.param( Tile::pReservoirWater ) <= 0 )
    {
      text = "##fountain_will_soon_be_hooked##";
    }
    else if( fountain->isActive() )
    {     
      text = fountain->mayWork()
              ? "##fountain_info##"
              : "##fountain_not_work##";
    }
    else
    {
      text = fountain->haveReservoirAccess()
               ? "##need_full_reservoir_for_work##"
               : "##need_reservoir_for_work##";
    }
  }

  _lbTextRef()->setText( _(text) );
}

AboutFontain::~AboutFontain(){}

void AboutFontain::_showHelp()
{
  DictionaryWindow::show( parent(), objects::fountain );
}

AboutReservoir::AboutReservoir(Widget* parent, PlayerCityPtr city, const Tile& tile)
  : AboutConstruction( parent, Rect( 0, 0, 480, 320 ), Rect( 0, 0, 1, 1 ) )
{
  setTitle( _("##reservoir##") );

  _lbTextRef()->setGeometry( Rect( 25, 45, width() - 25, height() - 55 ) );
  _lbTextRef()->setWordwrap( true );

  ReservoirPtr reservoir = ptr_cast<Reservoir>( tile.overlay() );
  setBase( ptr_cast<Construction>( reservoir ) );

  std::string text;
  if( reservoir.isValid() )
  {
    text = reservoir->haveWater()
              ? "##reservoir_info##"
              : "##reservoir_no_water##";
  }

  _lbTextRef()->setText( _(text) );
}

AboutReservoir::~AboutReservoir() {}

void AboutReservoir::_showHelp()
{
  DictionaryWindow::show( parent(), objects::reservoir );
}

AboutWell::AboutWell(Widget* parent, PlayerCityPtr city, const Tile& tile)
  : AboutConstruction( parent, Rect( 0, 0, 480, 320 ), Rect() )
{
  setTitle( _("##well##") );

  _lbTextRef()->setGeometry( Rect( 25, 45, width() - 25, height() - 55 ) );
  _lbTextRef()->setWordwrap( true );

  WellPtr well = ptr_cast<Well>( tile.overlay() );
  setBase( ptr_cast<Construction>( well ) );

  std::string text;
  if( well.isValid() )
  {
    TilesArray coverageArea = well->coverageArea();

    bool haveHouseInArea = false;
    foreach( tile, coverageArea )
    {
      haveHouseInArea |= is_kind_of<House>( (*tile)->overlay() );
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
        HousePtr house = ptr_cast<House>( (*tile)->overlay() );
        if( house.isValid() )
        {
          houseNeedWell |= ( house->getServiceValue( Service::fountain ) == 0 );
        }
      }

      if( !houseNeedWell )
      {
        text = "##also_fountain_in_well_area##";
      }
      else
      {
        TilesArray tiles = well->coverageArea();
        bool haveLowHealthHouse = false;
        foreach( it, tiles )
        {
          HousePtr house = ptr_cast<House>( (*it)->overlay() );
          if( house.isValid() )
          {
            haveLowHealthHouse |= house->state( (Construction::Param)House::health ) < 10;
          }
        }

        text = haveLowHealthHouse
                ? "##well_infected_info##"
                : "##well_info##";
      }
    }
  }

  _lbTextRef()->setText( _(text) );
}

AboutWell::~AboutWell() {}

void AboutWell::_showHelp()
{
  DictionaryWindow::show( parent(), objects::well );
}

}

}//end namespace gui
