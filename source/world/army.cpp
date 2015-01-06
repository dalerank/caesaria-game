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

#include "army.hpp"
#include "core/foreach.hpp"
#include "game/gamedate.hpp"
#include "empire.hpp"
#include "core/logger.hpp"
#include "city.hpp"
#include "empiremap.hpp"
#include "gfx/tilesarray.hpp"
#include "game/resourcegroup.hpp"
#include "core/variant_map.hpp"

using namespace gfx;

namespace world
{

class Army::Impl
{
public:
  CityPtr base;
  std::string destination;
  int strength;

  VariantMap options;
};

Army::Army( EmpirePtr empire )
  : MovableObject( empire ), __INIT_IMPL(Army)
{
  __D_IMPL(d,Army)

  _animation().load( ResourceGroup::empirebits, 37, 16 );
  _animation().setLoop( Animation::loopAnimation );
  Size size = _animation().frame( 0 ).size();
  _animation().setOffset( Point( -size.width() / 2, size.height() / 2 ) );
  d->strength = 0;
}

ArmyPtr Army::create(EmpirePtr empire)
{
  ArmyPtr ret( new Army( empire ) );

  ret->drop();

  return ret;
}

Army::~Army(){}

void Army::_reachedWay()
{
  __D_IMPL(d,Army)

  ObjectPtr obj;
  if( !d->destination.empty() )
  {
    obj = empire()->findObject( d->destination );
  }
  else
  {
    ObjectList objs = empire()->findObjects( location(), 20 );
    objs.remove( this );

    if( !objs.empty() )
      obj = objs.front();
  }

  if( obj.isValid() )
  {
    obj->addObject( this );
  }
  else
  {
    Logger::warning( "!!!Army: nof found object with name " +(d->destination.empty() ? "NULL" : d->destination) );
  }

  MovableObject::_reachedWay();
}

void Army::save(VariantMap& stream) const
{
  MovableObject::save( stream );

  __D_IMPL_CONST(d,Army)
  stream[ "base"  ] = Variant( d->base.isValid() ? d->base->name() : "" );
  VARIANT_SAVE_STR_D( stream, d, destination )
  VARIANT_SAVE_ENUM_D( stream, d, strength )
}

void Army::load(const VariantMap& stream)
{
  MovableObject::load( stream );

  __D_IMPL(d,Army)
  d->base = empire()->findCity( d->options[ "base" ].toString() );  
  d->options = stream;

  VARIANT_LOAD_STR_D( d, destination, stream )
  VARIANT_LOAD_ANY_D( d, strength, stream )
}

std::string Army::type() const { return CAESARIA_STR_EXT(Army); }

void Army::setBase(CityPtr base){  _dfunc()->base = base;  }

void Army::attack(ObjectPtr obj)
{
  __D_IMPL(d,Army)
  if( d->base.isValid() && obj.isValid() )
  {
    d->destination = obj->name();
    _findWay( d->base->location(), obj->location() );

    if( _way().empty() )
    {
      Logger::warning( "Army: cannot find way from %s to %s", d->base->name().c_str(), obj->name().c_str() );
    }

    attach();
  }
  else
  {
    Logger::warningIf( d->base.isNull(), "Army: base is null" );
    Logger::warningIf( obj.isNull(), "Army: object for attack is null" );
  }
}

void Army::setStrength(int value)
{
  _dfunc()->strength = value;
  if( _dfunc()->strength <= 0 )
  {
    deleteLater();
  }
}

int Army::strength() const { return _dfunc()->strength; }
void Army::killSoldiers(int percent)
{
  _dfunc()->strength = _dfunc()->strength * percent / 100;
}

void Army::addObject(ObjectPtr obj )
{
  MovableObject::addObject( obj );

  ArmyPtr otherArmy = ptr_cast<Army>( obj );
  if( otherArmy.isValid() && _isAgressiveArmy( otherArmy ) )
  {
    int attackersLoss = 0;
    int selfLoss = 0;

    Army::battle( otherArmy->strength(), strength(), attackersLoss, selfLoss );

    otherArmy->killSoldiers(attackersLoss);
    killSoldiers(selfLoss);
    }
}

std::string Army::target() const { return _dfunc()->destination; }

void Army::battle(unsigned int attackers, unsigned int defenders, int& attackersLoss, int& deffLoss )
{
  int delimArmy2self = math::percentage( attackers, defenders );
  attackersLoss = 0;
  deffLoss = 0;

  if( delimArmy2self < 25 )
  {
    attackersLoss = 100;
    deffLoss = math::random( 10 );
  }
  else if( delimArmy2self <= 100 )
  {
    int minAtLoss = 100 - delimArmy2self;
    int randomAtLoss = math::random(100+delimArmy2self);
    attackersLoss = math::clamp<int>( randomAtLoss, minAtLoss, 100 );

    int minSelfLoss = math::random( attackersLoss );
    int randomSelfLoss = math::random( attackersLoss + delimArmy2self );
    deffLoss = math::clamp<int>( randomSelfLoss, minSelfLoss, 100 );
  }
  else if( delimArmy2self < 400 )
  {
     int minb=0;
     int pctAdvantage = math::percentage( attackers - defenders, attackers );
     if (pctAdvantage < 10) {  minb = 70; }
     else if (pctAdvantage < 50) { minb = 50; }
     else if (pctAdvantage < 100) { minb = 40; }
     else if (pctAdvantage < 150) { minb = 30; }
     else if (pctAdvantage < 300) { minb = 20; }
     else { minb = 15; }

     attackersLoss = math::clamp<int>( math::random( 100 ), 0, minb );
     deffLoss = math::clamp<int>( math::random( 100 ), 100 - minb, 100 );
  }
  else
  {
    attackersLoss = math::random( 10 );
    deffLoss = 100;
    }
}

bool Army::_isAgressiveArmy(ArmyPtr) const
{
  return true;
}

}
