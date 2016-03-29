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

#include "infobox_dock.hpp"
#include "label.hpp"
#include "objects/dock.hpp"
#include "good/store.hpp"
#include "core/gettext.hpp"
#include "good/helper.hpp"
#include "core/utils.hpp"
#include "core/metric.hpp"
#include "core/logger.hpp"
#include "game/infoboxmanager.hpp"

using namespace gfx;

namespace gui
{

namespace infobox
{

REGISTER_OBJECT_BASEINFOBOX(dock,AboutDock)

AboutDock::AboutDock(Widget* parent, PlayerCityPtr city, const Tile& tile )
  : AboutConstruction( parent, Rect( 0, 0, 510, 286 ), Rect( 16, 185, 510 - 16, 185 + 50) )
{
  setupUI( ":/gui/infoboxdock.gui" );

  DockPtr dock = tile.overlay<Dock>();

  if( !dock.isValid() )
  {
    Logger::warning( "AboutMarket: market is null tile at [{0},{1}]", tile.i(), tile.j() );
    return;
  }

  setBase( dock );
  _setWorkingVisible( true );

  Label& lbAbout = add<Label>( Rect( 15, 30, width() - 15, 50) );
  lbAbout.setWordwrap( true );
  lbAbout.setFont( "FONT_1" );
  lbAbout.setTextAlignment( align::upperLeft, align::upperLeft );

  setTitle( _( dock->info().prettyName() ) );

  lbAbout.setText( _( dock->numberWorkers() > 0 ? "##dock_about##" : "##dock_no_workers##" ) );
  updateStore( dock );

  _updateWorkersLabel( Point( 32, 8 ), 542, dock->maximumWorkers(), dock->numberWorkers() );
}

AboutDock::~AboutDock() {}

void AboutDock::drawGood( DockPtr dock, const good::Product &goodType, int index, int paintY )
{
  int startOffset = 25;

  int offset = ( width() - startOffset * 2 ) / 6;
  //std::string goodName = good::Helper::name( goodType );
  int qty = dock->exportStore().qty( goodType );
  std::string outText = utils::i2str( metric::Measure::convQty( qty ) );

  // pictures of goods
  Point pos( index * offset + startOffset, paintY );

  Label& lb = add<Label>( Rect( pos, pos + Point( 100, 24 )) );
  lb.setFont( "FONT_2" );
  lb.setIcon( good::Info( goodType ).picture() );
  lb.setText( outText );
  lb.setTextOffset( Point( 30, 0 ) );
}

void AboutDock::updateStore( DockPtr dock )
{
  int paintY = 100;
  drawGood( dock, good::wheat,     0, paintY );
  drawGood( dock, good::meat,      1, paintY);
  drawGood( dock, good::fruit,     2, paintY);
  drawGood( dock, good::vegetable, 3, paintY);

  paintY += 24;
  drawGood( dock, good::olive,  0, paintY);
  drawGood( dock, good::grape,  1, paintY);
  drawGood( dock, good::timber, 2, paintY);
  drawGood( dock, good::clay,   3, paintY);
  drawGood( dock, good::iron,   4, paintY);
  drawGood( dock, good::marble, 5, paintY);

  paintY += 24;
  drawGood( dock, good::pottery,    0, paintY);
  drawGood( dock, good::furniture,  1, paintY);
  drawGood( dock, good::oil,        2, paintY);
  drawGood( dock, good::wine,       3, paintY);
  drawGood( dock, good::weapon,     4, paintY);
  drawGood( dock, good::prettyWine, 5, paintY );
}

}//end namesapace infobox

}//end namespace gui
