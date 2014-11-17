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
// Copyright 2012-2014 Dalerank, dalerankn8@gmail.com

#include "circus_charioter.hpp"
#include "city/city.hpp"
#include "objects/hippodrome.hpp"
#include "pathway/pathway.hpp"
#include "gfx/tilemap.hpp"
#include "gfx/animation_bank.hpp"

using namespace constants;
using namespace gfx;

class CircusCharioter::Impl
{
public:
  Picture cartPicture;

public:
  const Picture& getCartPicture( Direction direction );
};

CircusCharioter::CircusCharioter(PlayerCityPtr city)
  : Walker( city ), _d( new Impl )
{
  _setType( walker::circusCharioter );
  setFlag( Walker::vividly, false );
}

void CircusCharioter::_addToCircus(HippodromePtr circus)
{
  Pathway path;
  Tilemap& tmap = _city()->tilemap();

  TilePos start = circus->pos();

  switch( circus->direction() )
  {
  case north:
    path.init( tmap.at( start + TilePos( 2, 2 ) ) );
    path.setNextDirection( tmap, northWest );
    for( int i=0; i<8; i++ ) path.setNextDirection( tmap, north );
    path.setNextDirection( tmap, northEast );
    path.setNextDirection( tmap, southEast );

    for( int i=0; i<8; i++ ) path.setNextDirection( tmap, south );
    path.setNextDirection( tmap, southWest );
  break;

  case west:
    path.init( tmap.at( start + TilePos( 2, 2 ) ) );
    path.setNextDirection( tmap, northEast );
    for( int i=0; i<8; i++ ) path.setNextDirection( tmap, east );
    path.setNextDirection( tmap, southEast );
    path.setNextDirection( tmap, southWest );

    for( int i=0; i<8; i++ ) path.setNextDirection( tmap, west );
    path.setNextDirection( tmap, northWest );
  break;

  default:
  break;
  }

  setPos( path.startPos() );
  setPathway( path );
  go();
}

const Picture& CircusCharioter::Impl::getCartPicture( Direction direction )
{
  if( !cartPicture.isValid() )
  {
    cartPicture = AnimationBank::getCart( AnimationBank::animCircusCart, 0, direction );
    cartPicture.addOffset( 8, -12 );
  }

  return cartPicture;
}

WalkerPtr CircusCharioter::create(PlayerCityPtr city, HippodromePtr circus)
{
  CircusCharioter* ch = new CircusCharioter( city );
  ch->_addToCircus( circus );

  WalkerPtr ret( ch );
  ret->drop();

  return ret;
}

CircusCharioter::~CircusCharioter()
{

}

void CircusCharioter::getPictures(Pictures& oPics)
{
  oPics.clear();

  // depending on the walker direction, the cart is ahead or behind
  switch (direction())
  {
  case constants::west:
  case constants::northWest:
  case constants::north:
  case constants::northEast:
    oPics.push_back( _d->getCartPicture( direction() ) );
    oPics.push_back( getMainPicture() );
  break;

  case constants::east:
  case constants::southEast:
    oPics.push_back( _d->getCartPicture( direction()  ) );
    oPics.push_back( getMainPicture() );
  break;

  case constants::south:
  case constants::southWest:
    oPics.push_back( getMainPicture() );
    oPics.push_back( _d->getCartPicture( direction() ) );
  break;

  default:
  break;
  }
}

void CircusCharioter::timeStep(const unsigned long time)
{
  Walker::timeStep( time );
}

void CircusCharioter::_reachedPathway()
{
  _updatePathway( pathway().copy( 0 ) );
  go();
}

void CircusCharioter::_changeDirection()
{
  _d->cartPicture = Picture::getInvalid();

  Walker::_changeDirection();
}
