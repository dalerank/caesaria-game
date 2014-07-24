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

#include "immigrant.hpp"
#include "core/position.hpp"
#include "objects/road.hpp"
#include "gfx/animation_bank.hpp"
#include "city/city.hpp"
#include "constants.hpp"
#include "city/statistic.hpp"
#include "corpse.hpp"
#include "game/resourcegroup.hpp"

using namespace constants;
using namespace gfx;

Immigrant::Immigrant( PlayerCityPtr city ) : Emigrant( city )
{
  CitizenGroup peoples;
  peoples[ CitizenGroup::matureMin ] = 2;
  peoples[ CitizenGroup::childMin ] = 2;
  setPeoples( peoples );

  _setType( walker::immigrant );
}

const Picture& Immigrant::_cartPicture()
{
  if( !Emigrant::_cartPicture().isValid() )
  {
    _setCartPicture( AnimationBank::getCart( G_EMIGRANT_CART1, direction()) );
  }

  return Emigrant::_cartPicture();
}

void Immigrant::getPictures( Pictures& oPics)
{
  oPics.clear();

  // depending on the walker direction, the cart is ahead or behind
  switch (direction())
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

void Immigrant::_changeDirection()
{
  Emigrant::_changeDirection();
  _setCartPicture( Picture() );  // need to get the new graphic
}

void Immigrant::_updateThinks()
{
  StringArray thinks;
  thinks << "##immigrant_where_my_home##";
  thinks << "##immigrant_want_to_be_liontamer##";

  int fstock = city::Statistic::getFoodStock( _city() );
  int mconsumption = city::Statistic::getFoodMonthlyConsumption( _city() );
  if( fstock / (mconsumption+1) > 4 )
  {
    thinks << "##immigrant_much_food_here##";
  }

  setThinks( thinks.random() );
}

void Immigrant::timeStep(const unsigned long time)
{
  Walker::timeStep(time);
}

bool Immigrant::die()
{
  bool created = Walker::die();

  if( !created )
  {
    Corpse::create( _city(), pos(), ResourceGroup::citizen1, 1129, 1136 );
    return true;
  }

  return false;
}

ImmigrantPtr Immigrant::create(PlayerCityPtr city )
{
  ImmigrantPtr newEmigrant( new Immigrant( city ) );
  newEmigrant->drop();
  return newEmigrant;
}

Immigrant::~Immigrant(){}
