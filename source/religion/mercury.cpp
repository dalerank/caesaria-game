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
#include "events/showinfobox.hpp"
#include "game/gamedate.hpp"
#include "core/gettext.hpp"
#include "good/goodstore.hpp"

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

void Mercury::_doWrath(PlayerCityPtr city)
{
  events::GameEventPtr event = events::ShowInfobox::create( _("##wrath_of_mercury_title##"),
                                                            _("##wrath_of_mercury_description##"),
                                                            events::ShowInfobox::send2scribe );
  event->dispatch();

  WarehouseList whs;
  whs << city->overlays();

  foreach( it, whs )
  {
    GoodStore& store = (*it)->store();
    for( int i=Good::none; i < Good::goodCount; i++ )
    {
      Good::Type gtype = (Good::Type)i;
      int goodQty = math::random( store.qty( gtype ) );
      if( goodQty > 0 )
      {
        GoodStock rmStock( gtype, goodQty );
        store.retrieve( rmStock, goodQty );
      }
    }
  }
}

void Mercury::_doSmallCurse(PlayerCityPtr city)
{

}

void Mercury::_doBlessing(PlayerCityPtr city)
{

}

}//end namespace rome

}//end namespace religion
