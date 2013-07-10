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



#include "oc3_player.hpp"
#include "oc3_variant.hpp"


Player::Player()
{
   _funds = 0;
   _salary = 0;
}


void Player::save( VariantMap& stream ) const
{
  stream[ "money" ] = _funds;
  stream[ "name" ] = Variant( _name );
  stream[ "salary" ] = _salary;
}

void Player::load( const VariantMap& stream )
{
  _funds = stream.get( "money" ).toInt();
  _name = stream.get( "name" ).toString();
  _salary = stream.get( "salary" ).toInt(); 
}

void Player::appendMoney( int money )
{
  _funds += money;
}

int Player::getMoney() const
{
  return _funds;
}

void Player::setName( const std::string& name )
{
  _name = name;
}

std::string Player::getName() const
{
  return _name;
}

int Player::getSalary() const
{
  return _salary;
}

void Player::setSalary( const int value )
{
  _salary = value;
}