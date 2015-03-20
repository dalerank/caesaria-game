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
// Copyright 2012-2015 Dalerank, dalerankn8@gmail.com

#include "gift.hpp"
#include "gamedate.hpp"

class Gift::Impl
{
public:
  DateTime time;
  std::string name;
  std::string sender;
  int value;
};

const std::string& Gift::name() const { return _d->name; }
const std::string &Gift::sender() const { return _d->sender; }
int Gift::value() const {  return _d->value; }

void Gift::load(const VariantMap& stream)
{
  VARIANT_LOAD_TIME_D( _d, time, stream )
  VARIANT_LOAD_ANY_D( _d, value, stream )
  VARIANT_LOAD_STR_D( _d, name, stream )
  VARIANT_LOAD_STR_D( _d, sender, stream )
}

VariantMap Gift::save() const
{
  VariantMap ret;
  VARIANT_SAVE_STR_D( ret, _d, name )
  VARIANT_SAVE_STR_D( ret, _d, sender )
  VARIANT_SAVE_ANY_D( ret, _d, value )
  VARIANT_SAVE_ANY_D( ret, _d, time )

  return ret;
}

Gift::Gift(const std::string& sender, const std::string& name, int money, int data )
  : _d( new Impl )
{
  _d->sender = sender;
  _d->time = game::Date::current();
  _d->name = name;
  _d->value = money;
}

Gift::~Gift() {}

