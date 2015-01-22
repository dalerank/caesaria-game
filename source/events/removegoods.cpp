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

#include "removegoods.hpp"
#include "good/goodstore.hpp"
#include "objects/warehouse.hpp"
#include "objects/granary.hpp"
#include "city/helper.hpp"
#include "core/variant_map.hpp"
#include "game/game.hpp"

using namespace constants;

namespace events
{

GameEventPtr RemoveGoods::create(good::Product type, int qty  )
{
  RemoveGoods* r = new RemoveGoods();
  r->_qty = qty;
  r->_type = type;

  GameEventPtr ret( r );
  ret->drop();

  return ret;
}

template<class T>
void _removeGoodFrom( PlayerCityPtr city, objects::Type btype, good::Product what, int& qty )
{
  SmartList<T> bList;	
#ifdef CAESARIA_PLATFORM_HAIKU
  bList << city->overlays();
#else
  city::Helper helper( city );
  bList = helper.find<T>( btype );
#endif
  foreach( it, bList )
  {
    if( qty <= 0 )
      break;

    good::Store& store = (*it)->store();
    int maxQty = std::min( store.getMaxRetrieve( what ), qty );

    if( maxQty > 0 )
    {
      good::Stock stock( what, maxQty );
      store.retrieve( stock, maxQty );
      qty -= maxQty;
    }
  }
}

void RemoveGoods::_exec( Game& game, unsigned int time )
{
  _removeGoodFrom<Warehouse>( game.city(), objects::warehouse, _type, _qty );
  _removeGoodFrom<Granary>( game.city(), objects::granery, _type, _qty );
}

bool RemoveGoods::_mayExec(Game&, unsigned int) const { return true; }
bool RemoveGoods::isDeleted() const { return true; }

void RemoveGoods::load(const VariantMap& stream)
{
}

VariantMap RemoveGoods::save() const
{
  VariantMap ret;
  return ret;
}

RemoveGoods::RemoveGoods(){}

}//end namespace events
