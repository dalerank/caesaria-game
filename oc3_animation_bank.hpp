// This file is part of openCaesar3.
//
// openCaesar3 is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// openCaesar3 is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with openCaesar3.  If not, see <http://www.gnu.org/licenses/>.
//
// Copyright 2012-2013 Gregoire Athanase, gathanase@gmail.com

#ifndef __OPENCAESAR3_ANIMATION_BANK_H_INCLUDED__
#define __OPENCAESAR3_ANIMATION_BANK_H_INCLUDED__

#include "oc3_enums.hpp"
#include "oc3_animation.hpp"
#include "oc3_walker_action.hpp"
#include "oc3_good.hpp"

#include <map>

class AnimationBank
{
public:
  typedef std::map<WalkerAction, Animation> WalkerAnimationMap;

  static AnimationBank& instance();

  // loads all cart graphics
  static void loadCarts();
  static void loadWalkers();

  static const Picture& getCart( int cartID, const DirectionType& direction );

  static const WalkerAnimationMap& getWalker( const WalkerGraphicType walkerGraphic );

private:
  AnimationBank();

  class Impl;
  ScopedPtr< Impl > _d;
};

#endif  //__OPENCAESAR3_ANIMATION_BANK_H_INCLUDED__
