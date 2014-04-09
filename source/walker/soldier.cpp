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

#include "soldier.hpp"
#include "city/city.hpp"

using namespace constants;

class Soldier::Impl
{
public:
  float strikeForce;
  float resistance;
  int morale;

  Impl() : strikeForce ( 3.f ), resistance( 1.f ), morale( 0 ) {}
};

Soldier::Soldier(PlayerCityPtr city, walker::Type type)
    : Walker( city ), __INIT_IMPL(Soldier)
{
  _setType( type );
}

float Soldier::resistance() const { return _dfunc()->resistance; }
void Soldier::setResistance(float value) { _dfunc()->resistance = value;  }

float Soldier::strike() const { return _dfunc()->strikeForce; }
void Soldier::setStrike(float value) { _dfunc()->strikeForce = value; }

int Soldier::morale() const { return _dfunc()->morale; }

void Soldier::initialize(const VariantMap &options)
{
  Walker::initialize( options );
  setResistance( options.get( "resistance", 1.f ) );
  setStrike( options.get( "strike", 3.f ) );
}

Soldier::~Soldier() {}

