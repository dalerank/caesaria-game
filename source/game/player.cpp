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



#include "player.hpp"
#include "core/variant.hpp"


class Player::Impl
{
public:
  int funds;  // amount of money
  std::string name;
  int salary;
};

Player::Player() : _d( new Impl )
{
   _d->funds = 0;
   _d->salary = 0;
}

PlayerPtr Player::create()
{
  PlayerPtr ret( new Player() );
  ret->drop(); //delete automatically

  return ret;
}

void Player::save( VariantMap& stream ) const
{
  stream[ "money" ] = _d->funds;
  stream[ "name" ] = Variant( _d->name );
  stream[ "salary" ] = _d->salary;
}

void Player::load( const VariantMap& stream )
{
  _d->funds = (int)stream.get( "money" );
  _d->name = stream.get( "name" ).toString();
  _d->salary = (int)stream.get( "salary" );
}

void Player::appendMoney( int money )
{
  _d->funds += money;
}

int Player::getMoney() const
{
  return _d->funds;
}

Player::~Player()
{

}

void Player::setName( const std::string& name )
{
  _d->name = name;
}

std::string Player::getName() const
{
  return _d->name;
}

int Player::getSalary() const
{
  return _d->salary;
}

void Player::setSalary( const int value )
{
  _d->salary = value;
}
