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

#ifndef __CAESARIA_CARTANIMATION_H_INCLUDE_
#define __CAESARIA_CARTANIMATION_H_INCLUDE_

#include "animation.hpp"
#include "good/good.hpp"
#include "core/direction.hpp"

namespace gfx
{

class CartAnimation : public Animation
{
public:
  CartAnimation();
  virtual ~CartAnimation();

  void load(const good::Stock& stock, constants::Direction direction);
  void load(int animIndex, constants::Direction direction );
  bool isBack() const;

private:
  bool _isBack;
};

}//end namespace gfx
#endif //__CAESARIA_CARTANIMATION_H_INCLUDE_
