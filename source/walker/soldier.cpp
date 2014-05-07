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

#include "soldier.hpp"
#include "city/helper.hpp"

using namespace constants;
using namespace gfx;

class Soldier::Impl
{
public:
  Soldier::SldrAction action;
  float strikeForce;
  float resistance;
  unsigned int attackDistance;
  int morale;

  Impl() : strikeForce ( 3.f ), resistance( 1.f ),
           attackDistance( 1 ), morale( 0 ) {}
};

Soldier::Soldier(PlayerCityPtr city, walker::Type type)
    : Walker( city ), __INIT_IMPL(Soldier)
{
  _setType( type );
}

void Soldier::fight()
{
  setSpeed( 0.f );
  _setAction( acFight );
}

float Soldier::resistance() const { return _dfunc()->resistance; }
void Soldier::setResistance(float value) { _dfunc()->resistance = value;  }

float Soldier::strike() const { return _dfunc()->strikeForce; }
void Soldier::setStrike(float value) { _dfunc()->strikeForce = value; }

int Soldier::morale() const { return _dfunc()->morale; }

void Soldier::wait(int ticks)
{
  if( ticks < 0 )
  {
    _setSubAction( doNothing );
  }
  Walker::wait( ticks );
}

void Soldier::initialize(const VariantMap &options)
{
  Walker::initialize( options );
  setResistance( options.get( "resistance", 1.f ) );
  setStrike( options.get( "strike", 3.f ) );

  Variant ad = options.get( "attackDistance" );
  if( ad.isValid() )
  {
    setAttackDistance( math::clamp( ad.toInt(), 1, 99 ) );
  }
}

bool Soldier::_move2freePos( TilePos target )
{
  const int defaultRange = 10;
  city::Helper helper( _city() );
  Pathway way2freeslot = helper.findFreeTile<Soldier>( target, pos(), defaultRange );
  if( way2freeslot.isValid() )
  {
    _updatePathway( way2freeslot );
    go();
    _setSubAction( go2position );
    return true;
  }

  return false;
}


Soldier::~Soldier() {}

unsigned int Soldier::attackDistance() const{ return _dfunc()->attackDistance; }

Soldier::SldrAction Soldier::_subAction() const { return _dfunc()->action; }
void Soldier::_setSubAction(Soldier::SldrAction action){ _dfunc()->action = action; }
void Soldier::setAttackDistance(unsigned int distance) { _dfunc()->attackDistance = distance; }
