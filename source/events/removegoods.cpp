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

using namespace constants;

namespace events
{

GameEventPtr RemoveGoods::create( Good::Type type, int qty  )
{
  RemoveGoods* r = new RemoveGoods();
  r->_qty = qty;
  r->_type = type;

  GameEventPtr ret( r );
  ret->drop();

  return ret;
}

void RemoveGoods::_exec( Game& game, unsigned int time )
{

}

bool RemoveGoods::_mayExec(Game&, unsigned int) const { return true; }
bool RemoveGoods::isDeleted() const {  return true; }

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
