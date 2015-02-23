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
// Copyright 2012-2015 Dalerank, dalerankn8@gmail.com

#include "cart_animation.hpp"
#include "core/position.hpp"
#include "core/variant_map.hpp"
#include "core/foreach.hpp"
#include "core/utils.hpp"
#include "core/logger.hpp"
#include "good/stock.hpp"
#include "animation_bank.hpp"

namespace gfx
{

CartAnimation::CartAnimation()
{
}

CartAnimation::~CartAnimation(){}

void CartAnimation::load(const good::Stock &stock, Direction direction)
{
  int index = (stock.empty() ? good::none : stock.type()).toInt();
  *((Animation*)this) = AnimationBank::getCart( index, stock.capacity(), direction, _isBack );
}

void CartAnimation::load(int animIndex, Direction direction)
{
  *((Animation*)this) = AnimationBank::getCart( animIndex, 0, direction, _isBack );
}

bool CartAnimation::isBack() const { return _isBack; }

}//end namespace gfx
