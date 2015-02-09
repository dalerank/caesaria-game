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

#include "infobox_factory.hpp"
#include "core/utils.hpp"
#include "core/gettext.hpp"
#include "label.hpp"
#include "image.hpp"
#include "good/goodhelper.hpp"
#include "dictionary.hpp"
#include "environment.hpp"
#include "objects/shipyard.hpp"
#include "objects/wharf.hpp"
#include "core/logger.hpp"
#include "widget_helper.hpp"
#include "city/city.hpp"

using namespace constants;
using namespace gfx;

namespace gui
{

namespace infobox
{

AboutFactory::AboutFactory(Widget* parent, PlayerCityPtr city, const Tile& tile)
  : AboutConstruction( parent, Rect( 0, 0, 510, 256 ), Rect( 16, 160, 510 - 16, 160 + 42) )
{
  setupUI( ":/gui/infoboxfactory.gui" );

  FactoryPtr factory = ptr_cast<Factory>( tile.overlay() );
  setBase( ptr_cast<Construction>( factory ) );
  _type = factory->type();
  std::string  title = MetaDataHolder::findPrettyName( factory->type() );
  setTitle( _(title) );
  _setWorkingVisible( true );

  if( !factory.isValid() )
  {
    Logger::warning( "AboutFactory: cant convert base to factory at [%d,%d]", tile.i(), tile.j() );
    deleteLater();
    return;
  }

  // paint progress
  std::string text = utils::format( 0xff, "%s %d%%", _("##rawm_production_complete_m##"), factory->progress() );
  Size lbSize( (width() - 20) / 2, 25 );
  _lbProduction = new Label( this, Rect( _lbTitleRef()->leftbottom() + Point( 10, 0 ), lbSize ), text );
  _lbProduction->setFont( Font::create( FONT_2 ) );

  std::string effciencyText = utils::format( 0xff, "%s %d%%", _("##effciency##"), factory->effciency() );
  _lbEffciency = new Label( this, _lbProduction->relativeRect() + Point( lbSize.width(), 0 ), effciencyText );
  _lbEffciency->setFont( Font::create( FONT_2 ) );


  if( factory->produceGoodType() != good::none )
  {
    new Image( this, Point( 10, 10), good::Helper::picture( factory->produceGoodType() ) );
  }

  // paint picture of in good
  if( factory->inStockRef().type() != good::none )
  {
    Label* lbStockInfo = new Label( this, Rect( _lbTitleRef()->leftbottom() + Point( 0, 25 ), Size( width() - 32, 25 ) ) );
    lbStockInfo->setIcon( good::Helper::picture( factory->inStockRef().type() ) );

    std::string whatStock = utils::format( 0xff, "##%s_factory_stock##", good::Helper::getTypeName( factory->inStockRef().type() ).c_str() );
    std::string text = utils::format( 0xff, "%d %s %s %d",
                                             factory->inStockRef().qty() / 100,
                                             _(whatStock),
                                             _("##factory_units##"),
                                             factory->outStockRef().qty() / 100 );

    lbStockInfo->setText( text );
    lbStockInfo->setTextOffset( Point( 30, 0 ) );

    _lbTextRef()->setPosition( lbStockInfo->leftbottom() + Point( 0, 5 ));
  }

  std::string workInfo = factory->workersProblemDesc();
  std::string cartInfo = factory->cartStateDesc();
  setText( utils::format( 0xff, "%s\n%s", _(workInfo), _( cartInfo ) ) );

  _updateWorkersLabel( Point( 32, 157 ), 542, factory->maximumWorkers(), factory->numberWorkers() );
}

void AboutFactory::_showHelp()
{
  DictionaryWindow::show( ui()->rootWidget(), _type );
}

AboutShipyard::AboutShipyard(Widget* parent, PlayerCityPtr city, const Tile& tile)
  : AboutFactory( parent, city, tile )
{
  ShipyardPtr shipyard = ptr_cast<Shipyard>( tile.overlay() );

  int progressCount = shipyard->progress();
  if( progressCount > 1 && progressCount < 100 )
  {
    Label* lb = new Label( this,
                           Rect( _lbProduction->leftbottom() + Point( 0, 5 ), Size( width() - 90, 25 ) ),
                           _("##build_fishing_boat##") );
    lb->setTextAlignment( align::upperLeft, align::upperLeft );
    _lbTextRef()->setPosition( lb->leftbottom() + Point( 0, 5 ) );
  }
}


AboutWharf::AboutWharf(Widget* parent, PlayerCityPtr city, const Tile& tile)
  : AboutFactory( parent, city, tile )
{
  WharfPtr wharf = ptr_cast<Wharf>( tile.overlay() );

  if( wharf->getBoat().isNull() )
  {
    Label* lb = new Label( this,
                           Rect( _lbProduction->leftbottom() + Point( 0, 10 ), Size( width() - 90, 25 ) ),
                           _("##wait_for_fishing_boat##") );
    lb->setTextAlignment( align::upperLeft, align::upperLeft );
    lb->setWordwrap( true );
    _lbTextRef()->setPosition( lb->leftbottom() + Point( 0, 10 ) );
  }
}

}

}//end namespace gui
