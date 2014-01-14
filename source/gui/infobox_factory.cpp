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

using namespace constants;

namespace gui
{

InfoboxFactory::InfoboxFactory( Widget* parent, const Tile& tile)
  : InfoBoxSimple( parent, Rect( 0, 0, 510, 256 ), Rect( 16, 147, 510 - 16, 147 + 62) )
{
  FactoryPtr factory = tile.getOverlay().as<Factory>();
  _type = factory->getType();
  setTitle( MetaDataHolder::getPrettyName( factory->getType() ) );

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
  setText( getInfoText( factory ) );

  _updateWorkersLabel( Point( 32, 157 ), 542, factory->getMaxWorkers(), factory->getWorkersCount() );
}

void InfoboxFactory::showDescription()
{
  DictionaryWindow::show( getEnvironment()->getRootWidget(), _type );
}

std::string InfoboxFactory::getInfoText( FactoryPtr factory )
{
  std::string factoryType = MetaDataHolder::getTypename( factory->getType() );
  float workKoeff = factory->getWorkersCount() * 100 / factory->getMaxWorkers();

  if( workKoeff == 0 )     {  return "##" + factoryType + "_no_workers##";      }
  else if(workKoeff < 25)  {  return "##" + factoryType + "_bad_work##";        }
  else if (workKoeff < 50) {  return "##" + factoryType + "_slow_work##";       }
  else if (workKoeff < 75) {  return "##" + factoryType + "_patrly_workers##";  }
  else if (workKoeff < 100 ){ return "##" + factoryType + "_need_some_workers##";  }
  else                     {  return "##" + factoryType + "_full_work##";       }
}

}//end namespace gui
