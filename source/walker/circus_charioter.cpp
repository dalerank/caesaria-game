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

using namespace gfx;

class CircusCharioter::Impl
{
public:
  Animation animation;

public:
  const Animation& cart( Direction direction );
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
  Tilemap& tmap = _map();

  TilePos start = circus->pos();

  switch( circus->direction() )
  {
  case direction::north:
    path.init( tmap.at( start + TilePos( 2, 2 ) ) );
    path.setNextDirection( tmap, direction::northWest );
    for( int i=0; i<8; i++ ) path.setNextDirection( tmap, direction::north );
    path.setNextDirection( tmap, direction::northEast );
    path.setNextDirection( tmap, direction::southEast );

    for( int i=0; i<8; i++ ) path.setNextDirection( tmap, direction::south );
    path.setNextDirection( tmap, direction::southWest );
  break;

  case direction::west:
    path.init( tmap.at( start + TilePos( 2, 2 ) ) );
    path.setNextDirection( tmap, direction::northEast );
    for( int i=0; i<8; i++ ) path.setNextDirection( tmap, direction::east );
    path.setNextDirection( tmap, direction::southEast );
    path.setNextDirection( tmap, direction::southWest );

    for( int i=0; i<8; i++ ) path.setNextDirection( tmap, direction::west );
    path.setNextDirection( tmap, direction::northWest );
  break;

  default:
  break;
  }

  setPos( path.startPos() );
  setPathway( path );
  go();
}

const Animation& CircusCharioter::Impl::cart( Direction direction )
{
  if( !animation.isValid() )
  {
    bool isBack;
    animation = AnimationBank::getCart( AnimationBank::animCircusCart, 0, direction, isBack );
    animation.addOffset( Point( 8, -12 ) );
  }

  return animation;
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
  case direction::west:
  case direction::northWest:
  case direction::north:
  case direction::northEast:
    oPics.push_back( _d->cart( direction() ).currentFrame() );
    oPics.push_back( getMainPicture() );
  break;

  case direction::east:
  case direction::southEast:
    oPics.push_back( _d->cart( direction()  ).currentFrame() );
    oPics.push_back( getMainPicture() );
  break;

  case direction::south:
  case direction::southWest:
    oPics.push_back( getMainPicture() );
    oPics.push_back( _d->cart( direction() ).currentFrame() );
  break;

  default:
  break;
  }
}

void CircusCharioter::timeStep(const unsigned long time)
{
  _d->animation.update( time );
  Walker::timeStep( time );
}

void CircusCharioter::_reachedPathway()
{
  _updatePathway( pathway().copy( 0 ) );
  go();
}

void CircusCharioter::_changeDirection()
{
  _d->animation = Animation();

  Walker::_changeDirection();
}
