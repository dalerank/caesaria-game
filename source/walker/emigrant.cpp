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

#include "emigrant.hpp"
#include "core/position.hpp"
#include "game/road.hpp"
#include "gfx/animation_bank.hpp"
#include "game/city.hpp"
#include "constants.hpp"
#include "corpse.hpp"
#include "game/resourcegroup.hpp"

using namespace constants;

Emigrant::Emigrant( CityPtr city ) : Immigrant( city )
{
  CitizenGroup peoples;
  peoples[ CitizenGroup::matureMin ] = 2;
  peoples[ CitizenGroup::childMin ] = 2;
  setPeoples( peoples );

  _setType( walker::emigrant );
  _setGraphic( WG_PUSHER2 );
}

const Picture& Emigrant::getCartPicture()
{
  if( !Immigrant::getCartPicture().isValid() )
  {
    setCartPicture( AnimationBank::getCart( G_EMIGRANT_CART1, getDirection()) );
  }

  return Immigrant::getCartPicture();
}

void Emigrant::getPictureList(std::vector<Picture> &oPics)
{
  oPics.clear();

  // depending on the walker direction, the cart is ahead or behind
  switch (getDirection())
  {
  case D_WEST:
  case D_NORTH_WEST:
  case D_NORTH:
  case D_NORTH_EAST:
    oPics.push_back( getCartPicture() );
    oPics.push_back( getMainPicture() );
    break;
  case D_EAST:
  case D_SOUTH_EAST:
    oPics.push_back( getCartPicture() );
    oPics.push_back( getMainPicture() );
    break;
  case D_SOUTH:
  case D_SOUTH_WEST:
    oPics.push_back( getMainPicture() );
    oPics.push_back( getCartPicture() );
    break;
  default:
    break;
  }
}

void Emigrant::onNewDirection()
{
  Immigrant::onNewDirection();
  setCartPicture( Picture() );  // need to get the new graphic
}

void Emigrant::die()
{
  Walker::die();

  Corpse::create( _getCity(), getIJ(), ResourceGroup::citizen1, 1129, 1136 );
}

EmigrantPtr Emigrant::create( CityPtr city )
{
  EmigrantPtr newEmigrant( new Emigrant( city ) );
  newEmigrant->drop();
  return newEmigrant;
}

Emigrant::~Emigrant()
{

}
