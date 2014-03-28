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

  Impl() : strikeForce ( 3.f ), resistance( 1.f ) {}
};

Soldier::Soldier(PlayerCityPtr city, walker::Type type)
    : Walker( city ), __INIT_IMPL(Soldier)
{
  _setType( type );
}

float Soldier::resistance() const { __D_IMPL_CONST(_d,Soldier); return _d->resistance; }
void Soldier::setResistance(float value) {  __D_IMPL(_d,Soldier); _d->resistance = value;  }
float Soldier::strike() const { __D_IMPL_CONST(_d,Soldier); return _d->strikeForce; }
void Soldier::setStrike(float value) { __D_IMPL(_d,Soldier); _d->strikeForce = value; }

void Soldier::initialize(const VariantMap &options)
{
  Walker::initialize( options );
  setResistance( options.get( "resistance", 1.f ) );
  setStrike( options.get( "strike", 3.f ) );
}

Soldier::~Soldier() {}

