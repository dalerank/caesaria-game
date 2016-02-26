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

#include "fundissue.hpp"
#include "game/funds.hpp"
#include "game/game.hpp"
#include "city/city.hpp"
#include "good/helper.hpp"
#include "city/trade_options.hpp"

using namespace econ;

namespace events
{

GameEventPtr Payment::create(int type, int value)
{
  Payment* ev = new Payment();
  ev->_value = value;
  ev->_type = type;
  GameEventPtr ret( ev );
  ret->drop();
  return ret;
}

GameEventPtr Payment::import(good::Product good, int qty, float buff)
{
  Payment* ev = new Payment();
  ev->_gtype = good;
  ev->_qty = qty;
  ev->_buff = buff;
  ev->_type = Issue::importGoods;
  GameEventPtr ret( ev );
  ret->drop();
  return ret;
}

GameEventPtr Payment::exportg(good::Product good, int qty, float buff)
{
  Payment* ev = new Payment();
  ev->_gtype = good;
  ev->_qty = qty;
  ev->_buff = buff;
  ev->_type = Issue::exportGoods;
  GameEventPtr ret( ev );
  ret->drop();
  return ret;
}

bool Payment::_mayExec(Game& game, unsigned int time) const {  return true; }

Payment::Payment() : _type( 0 ), _value( 0 ), _buff( 1.f ), _gtype( good::none ), _qty( 0 )
{}

void Payment::_exec(Game& game, unsigned int )
{
  if( _type == Issue::importGoods )
  {
    int price = good::Helper::importPrice( game.city(), _gtype, _qty );
    _value = (int)(-price * ( 1+_buff ));
  }
  else if( _type == Issue::exportGoods )
  {
    int price = good::Helper::exportPrice( game.city(), _gtype, _qty );
    _value = price * ( 1+_buff );
  }

  game.city()->treasury().resolveIssue( Issue( (Issue::Type)_type, _value ) );
}

} //end namespace events
