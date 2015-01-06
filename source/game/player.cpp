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
// Copyright 2012-2013 Gregoire Athanase, gathanase@gmail.com
// Copyright 2012-2014 Dalerank, dalerankn8@gmail.com

#include "player.hpp"
#include "core/variant_map.hpp"

class Player::Impl
{
public:
  int funds;  // amount of money
  std::string name;
  int salary;
  int rank;
  unsigned int color;
};

Player::Player() : _d( new Impl )
{
  _d->funds = 0;
  _d->salary = 0;
  _d->rank = 0;
}

PlayerPtr Player::create()
{
  PlayerPtr ret( new Player() );
  ret->drop(); //delete automatically

  return ret;
}

void Player::save( VariantMap& stream ) const
{
  VARIANT_SAVE_ANY_D( stream, _d, funds );
  VARIANT_SAVE_STR_D( stream, _d, name );
  VARIANT_SAVE_ANY_D( stream, _d, salary );
  VARIANT_SAVE_ANY_D( stream, _d, color );
  VARIANT_SAVE_ANY_D( stream, _d, rank );
}

void Player::load( const VariantMap& stream )
{
  VARIANT_LOAD_ANY_D( _d, funds, stream );
  VARIANT_LOAD_STR_D( _d, name, stream );
  VARIANT_LOAD_ANY_D( _d, salary, stream );
  VARIANT_LOAD_ANY_D( _d, color, stream );
  VARIANT_LOAD_ANY_D( _d, rank, stream );
}

void Player::appendMoney( int money ){  _d->funds += money;}
int Player::money() const{  return _d->funds;}

unsigned int Player::color() const{ return _d->color; }
Player::~Player(){}
void Player::setName( const std::string& name ){  _d->name = name;}
std::string Player::name() const{  return _d->name;}
void Player::setRank(int rank) { _d->rank = rank; }
int Player::rank() const{ return _d->rank; }
int Player::salary() const{  return _d->salary;}
void Player::setSalary(  int value ){  _d->salary = value;}
