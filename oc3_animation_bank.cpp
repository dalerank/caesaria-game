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

#include "oc3_animation_bank.hpp"
#include "oc3_positioni.hpp"
#include "oc3_resourcegroup.hpp"
#include "oc3_picture.hpp"

namespace{
  static const Point frontCartOffsetSouth = Point( -33, 22 );
  static const Point frontCartOffsetWest  = Point( -31, 35 );
  static const Point frontCartOffsetNorth = Point(  -5, 37 );
  static const Point frontCartOffsetEast  = Point(  -5, 22 );
  static const Point frontCartOffsetSouthEast  = Point( -20, 20 );
  static const Point frontCartOffsetNorthWest  = Point( -20, 40 );
  static const Point frontCartOffsetNorthEast  = Point( -5, 30 );
  static const Point frontCartOffsetSouthWest  = Point( -5, 22 );

  static const Point backCartOffsetSouth = Point(  -5, 40 );
  static const Point backCartOffsetWest  = Point(  -5, 22 );
  static const Point backCartOffsetNorth = Point( -33, 22 );
  static const Point backCartOffsetEast  = Point( -31, 35 );
  static const Point backCartOffsetSouthEast  = Point( -20, 40 );
  static const Point backCartOffsetNorthWest  = Point( -20, 20 );
  static const Point backCartOffsetNorthEast  = Point( -30, 30 );
  static const Point backCartOffsetSouthWest  = Point( -20, 20 );

  static const int noneGoodsPicId = 1;
}

class AnimationBank::Impl
{
public:
  std::vector< PicturesArray > carts; // pictures[GoodType][Direction]

  // fills the cart pictures
  // prefix: image prefix
  // start: index of the first frame
  PicturesArray fillCart( const std::string &prefix, const int start, bool back );
};

AnimationBank& AnimationBank::instance()
{
  static AnimationBank inst;
  return inst;
}

AnimationBank::AnimationBank() : _d( new Impl )
{
  loadAll();
}

void AnimationBank::loadAll()
{
  std::cout << "Loading cart graphics" << std::endl;

  //number of animations with goods + emmigrants + immigrants
  _d->carts.resize(CT_MAX);

  bool frontCart = false;

  _d->carts[G_NONE] = _d->fillCart(ResourceGroup::carts, noneGoodsPicId, frontCart);
  _d->carts[G_WHEAT] = _d->fillCart(ResourceGroup::carts, 9, frontCart);
  _d->carts[G_VEGETABLE] = _d->fillCart(ResourceGroup::carts, 17, frontCart);
  _d->carts[G_FRUIT] = _d->fillCart( ResourceGroup::carts, 25, frontCart);
  _d->carts[G_OLIVE] = _d->fillCart( ResourceGroup::carts, 33, frontCart);
  _d->carts[G_GRAPE] = _d->fillCart( ResourceGroup::carts, 41, frontCart);
  _d->carts[G_MEAT] = _d->fillCart( ResourceGroup::carts, 49, frontCart);
  _d->carts[G_WINE] = _d->fillCart( ResourceGroup::carts, 57, frontCart);
  _d->carts[G_OIL] = _d->fillCart( ResourceGroup::carts, 65, frontCart);
  _d->carts[G_IRON] = _d->fillCart( ResourceGroup::carts, 73, frontCart);
  _d->carts[G_TIMBER] = _d->fillCart( ResourceGroup::carts, 81, frontCart);
  _d->carts[G_CLAY] =  _d->fillCart( ResourceGroup::carts, 89, frontCart);
  _d->carts[G_MARBLE] = _d->fillCart( ResourceGroup::carts, 97, frontCart);
  _d->carts[G_WEAPON] = _d->fillCart( ResourceGroup::carts, 105, frontCart);
  _d->carts[G_FURNITURE] = _d->fillCart( ResourceGroup::carts, 113, frontCart);
  _d->carts[G_POTTERY] = _d->fillCart( ResourceGroup::carts, 121, frontCart);
  _d->carts[G_EMIGRANT_CART1] = _d->fillCart( ResourceGroup::carts, 129, !frontCart);
  _d->carts[G_ENIGRANT_CART2] = _d->fillCart( ResourceGroup::carts, 137, !frontCart);
  _d->carts[G_FISH] = _d->fillCart( ResourceGroup::carts, 697, frontCart);
}

PicturesArray AnimationBank::Impl::fillCart( const std::string &prefix, const int start, bool back )
{
  PicturesArray ioCart;

  ioCart.resize(D_MAX);

  ioCart[D_NORTH]      = &Picture::load(ResourceGroup::carts, start);
  ioCart[D_NORTH_EAST] = &Picture::load(ResourceGroup::carts, start + 1);
  ioCart[D_EAST]       = &Picture::load(ResourceGroup::carts, start + 2);
  ioCart[D_SOUTH_EAST] = &Picture::load(ResourceGroup::carts, start + 3);
  ioCart[D_SOUTH]      = &Picture::load(ResourceGroup::carts, start + 4);
  ioCart[D_SOUTH_WEST] = &Picture::load(ResourceGroup::carts, start + 5);
  ioCart[D_WEST]       = &Picture::load(ResourceGroup::carts, start + 6);
  ioCart[D_NORTH_WEST] = &Picture::load(ResourceGroup::carts, start + 7);

  ioCart[D_SOUTH]->setOffset( back ? backCartOffsetSouth : frontCartOffsetSouth);
  ioCart[D_WEST]->setOffset ( back ? backCartOffsetWest  : frontCartOffsetWest );
  ioCart[D_NORTH]->setOffset( back ? backCartOffsetNorth : frontCartOffsetNorth);
  ioCart[D_EAST]->setOffset ( back ? backCartOffsetEast  : frontCartOffsetEast );

  ioCart[D_SOUTH_EAST]->setOffset ( back ? backCartOffsetSouthEast  : frontCartOffsetSouthEast );
  ioCart[D_NORTH_WEST]->setOffset ( back ? backCartOffsetNorthWest  : frontCartOffsetNorthWest );
  ioCart[D_NORTH_EAST]->setOffset ( back ? backCartOffsetNorthEast  : frontCartOffsetNorthEast );
  ioCart[D_SOUTH_WEST]->setOffset ( back ? backCartOffsetSouthWest  : frontCartOffsetSouthWest );

  return ioCart;
}

Picture& AnimationBank::getCart(GoodType cart, const DirectionType &direction)
{
  Picture *res = NULL;
  res = _d->carts.at( cart ).at( direction );
  return *res;
}

Picture& AnimationBank::getCart(CartTypes cart, const DirectionType &direction)
{
  Picture *res = NULL;
  res = _d->carts.at( cart ).at( direction );
  return *res;
}
