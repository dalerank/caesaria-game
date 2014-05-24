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
// Copyright 2012-2014 dalerank, dalerankn8@gmail.com

#include "emigrant.hpp"
#include "core/position.hpp"
#include "objects/road.hpp"
#include "gfx/animation_bank.hpp"
#include "city/city.hpp"
#include "constants.hpp"
#include "corpse.hpp"
#include "game/resourcegroup.hpp"

using namespace constants;
using namespace gfx;

Emigrant::Emigrant( PlayerCityPtr city ) : Immigrant( city )
{
  CitizenGroup peoples;
  peoples[ CitizenGroup::matureMin ] = 2;
  peoples[ CitizenGroup::childMin ] = 2;
  setPeoples( peoples );

  _setType( walker::emmigrant );
}

const Picture& Emigrant::_cartPicture()
{
  if( !Immigrant::_cartPicture().isValid() )
  {
    _setCartPicture( AnimationBank::getCart( G_EMIGRANT_CART1, getDirection()) );
  }

  return Immigrant::_cartPicture();
}

void Emigrant::getPictures( Pictures& oPics)
{
  oPics.clear();

  // depending on the walker direction, the cart is ahead or behind
  switch (getDirection())
  {
  case constants::west:
  case constants::northWest:
  case constants::north:
  case constants::northEast:
    oPics.push_back( _cartPicture() );
    oPics.push_back( getMainPicture() );
  break;

  case constants::east:
  case constants::southEast:
    oPics.push_back( _cartPicture() );
    oPics.push_back( getMainPicture() );
  break;

  case constants::south:
  case constants::southWest:
    oPics.push_back( getMainPicture() );
    oPics.push_back( _cartPicture() );
  break;

  default:
  break;
  }
}

void Emigrant::_changeDirection()
{
  Immigrant::_changeDirection();
  _setCartPicture( Picture() );  // need to get the new graphic
}

void Emigrant::_updateThinks()
{
  StringArray thinks;
  thinks << "##emigrant_where_my_home##";

  setThinks( thinks.rand() );
}

void Emigrant::timeStep(const unsigned long time)
{
  Walker::timeStep(time);
}

void Emigrant::die()
{
  Walker::die();

  Corpse::create( _city(), pos(), ResourceGroup::citizen1, 1129, 1136 );
}

EmigrantPtr Emigrant::create(PlayerCityPtr city )
{
  EmigrantPtr newEmigrant( new Emigrant( city ) );
  newEmigrant->drop();
  return newEmigrant;
}

Emigrant::~Emigrant(){}
