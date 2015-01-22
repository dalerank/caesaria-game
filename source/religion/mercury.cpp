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

#include "city/city.hpp"
#include "mercury.hpp"
#include "objects/warehouse.hpp"
#include "objects/granary.hpp"
#include "events/showinfobox.hpp"
#include "game/gamedate.hpp"
#include "core/gettext.hpp"
#include "good/goodstore.hpp"
#include "objects/extension.hpp"
#include "objects/factory.hpp"
#include "core/utils.hpp"

using namespace constants;
using namespace gfx;

namespace religion
{

namespace rome
{

DivinityPtr Mercury::create()
{
  DivinityPtr ret( new Mercury() );
  ret->setInternalName( baseDivinityNames[ romeDivMercury ] );
  ret->drop();

  return ret;
}

void Mercury::updateRelation(float income, PlayerCityPtr city)
{
  RomeDivinity::updateRelation( income, city );
}

template<class T>
void __filchGoods( const std::string& title, PlayerCityPtr city, bool showMessage )
{
  if( showMessage )
  {
    std::string txt = utils::format( 0xff, "##%s_of_mercury_title##", title.c_str() );
    std::string descr = utils::format( 0xff, "##%s_of_mercury_description##", title.c_str() );

    events::GameEventPtr event = events::ShowInfobox::create( _(txt),
                                                              _(descr),
                                                              events::ShowInfobox::send2scribe,
                                                              ":/smk/God_Mercury.smk");
    event->dispatch();
  }

  SmartList<T> buildings;
  buildings << city->overlays();

  foreach( it, buildings )
  {
    good::Store& store = (*it)->store();
    for( good::Product gtype=good::wheat; gtype < good::goodCount; ++gtype )
    {
      int goodQty = math::random( (store.qty( gtype ) + 99) / 100 ) * 100;
      if( goodQty > 0 )
      {
        good::Stock rmStock( gtype, goodQty );
        store.retrieve( rmStock, goodQty );
      }
    }
  }
}

void Mercury::_doWrath(PlayerCityPtr city)
{
  __filchGoods<Warehouse>( "wrath", city, true );
  __filchGoods<Granary>( "smallcurse", city, false );
}

void Mercury::_doSmallCurse(PlayerCityPtr city)
{
  events::GameEventPtr event = events::ShowInfobox::create( _("##smallcurse_of_mercury_title##"),
                                                            _("##smallcurse_of_mercury_description##") );
  event->dispatch();

  FactoryList factories;
  factories << city->overlays();

  foreach( it, factories )
  {
    FactoryProgressUpdater::assignTo( ptr_cast<Factory>( *it ), -5, 4 * 12 );
  }
}

void Mercury::_doBlessing(PlayerCityPtr city)
{
  WarehouseList whList;
  whList << city->overlays();

  foreach( it, whList )
  {
    WarehouseBuff::assignTo( *it, Warehouse::sellGoodsBuff, 0.2, 4 * 12 );
  }
}

}//end namespace rome

}//end namespace religion
