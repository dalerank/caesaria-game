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

#include "infobox_factory.hpp"
#include "core/stringhelper.hpp"
#include "core/gettext.hpp"
#include "label.hpp"
#include "image.hpp"
#include "good/goodhelper.hpp"
#include "dictionary.hpp"
#include "environment.hpp"
#include "objects/shipyard.hpp"
#include "objects/wharf.hpp"

using namespace constants;

namespace gui
{

InfoboxFactory::InfoboxFactory( Widget* parent, const Tile& tile)
  : InfoBoxSimple( parent, Rect( 0, 0, 510, 256 ), Rect( 16, 147, 510 - 16, 147 + 62) )
{
  FactoryPtr factory = ptr_cast<Factory>( tile.getOverlay() );
  _type = factory->getType();
  std::string  title = MetaDataHolder::getPrettyName( factory->getType() );
  setTitle( _(title) );

  // paint progress
  std::string text = StringHelper::format( 0xff, "%s %d%%", _("##production_ready_at##"), factory->getProgress() );
  new Label( this, Rect( _getTitle()->getLeftdownCorner() + Point( 10, 0 ), Size( getWidth() - 32, 25 ) ), text );

  if( factory->getOutGoodType() != Good::none )
  {
    new Image( this, Point( 10, 10), GoodHelper::getPicture( factory->getOutGoodType() ) );
  }

  // paint picture of in good
  if( factory->inStockRef().type() != Good::none )
  {
    Label* lbStockInfo = new Label( this, Rect( _getTitle()->getLeftdownCorner() + Point( 0, 25 ), Size( getWidth() - 32, 25 ) ) );
    lbStockInfo->setIcon( GoodHelper::getPicture( factory->inStockRef().type() ) );

    std::string text = StringHelper::format( 0xff, "%s %s: %d %s",
                                             GoodHelper::getName( factory->inStockRef().type() ).c_str(),
                                             _("##factory_stock##"),
                                             factory->inStockRef().qty() / 100,
                                             _("##factory_units##") );

    lbStockInfo->setText( text );
    lbStockInfo->setTextOffset( Point( 30, 0 ) );
  }

  _getInfo()->move( Point( 0, 15 ));

  std::string workInfo = getInfoText( factory );
  setText( _(workInfo) );

  _updateWorkersLabel( Point( 32, 157 ), 542, factory->getMaxWorkers(), factory->getWorkersCount() );
}

void InfoboxFactory::showDescription()
{
  DictionaryWindow::show( getEnvironment()->getRootWidget(), _type );
}

std::string InfoboxFactory::getInfoText( FactoryPtr factory )
{
  std::string factoryType = MetaDataHolder::getTypename( factory->getType() );
  float workKoeff = (factory->getWorkersCount() / (float)factory->getMaxWorkers()) * 5.f;

  const char* workKoeffStr[] = { "_no_workers", "_bad_work", "_slow_work", "_patrly_workers",
                                 "_need_some_workers", "_full_work" };

  return StringHelper::format( 0xff, "##%s%s##", factoryType.c_str(), workKoeffStr[ (int)ceil(workKoeff) ] );
}

InfoboxShipyard::InfoboxShipyard(Widget* parent, const Tile& tile)
  : InfoboxFactory( parent, tile )
{
  ShipyardPtr shipyard = ptr_cast<Shipyard>( tile.getOverlay() );

  int progressCount = shipyard->getProgress();
  if( progressCount > 1 && progressCount < 100 )
  {
    new Label( this,
               Rect( _getTitle()->getLeftdownCorner() + Point( 10, 35 ), Size( getWidth() - 32, 25 ) ),
               _("##build_fishing_boat##") );
  }
}


InfoboxWharf::InfoboxWharf(Widget* parent, const Tile& tile)
  : InfoboxFactory( parent, tile )
{
  WharfPtr wharf = ptr_cast<Wharf>( tile.getOverlay() );

  if( wharf->getBoat().isNull() )
  {
    new Label( this,
               Rect( _getTitle()->getLeftdownCorner() + Point( 10, 35 ), Size( getWidth() - 32, 25 ) ),
               _("##wait_for_fishing_boat##") );
  }
}


}//end namespace gui
