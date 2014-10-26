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

#include "human.hpp"
#include "city/city.hpp"

using namespace constants;

class Human::Impl
{
public:
  walker::Nation nation;
};

walker::Nation Human::nation() const{ return _d->nation; }
void Human::_setNation(walker::Nation nation) { _d->nation = nation; }

Human::Human(PlayerCityPtr city)
  : Walker( city ), _d( new Impl )
{
  _d->nation = walker::unknownNation;
  setFlag( Walker::vividly, true );
}

Human::~Human()
{

}
