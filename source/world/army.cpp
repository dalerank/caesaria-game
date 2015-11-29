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
#include "game/gamedate.hpp"
#include "empire.hpp"
#include "core/logger.hpp"
#include "city.hpp"
#include "empiremap.hpp"
#include "gfx/tilesarray.hpp"
#include "game/resourcegroup.hpp"
#include "core/variant_map.hpp"
#include "config.hpp"

using namespace gfx;

namespace world
{

namespace {
static const int maxLoss = 100;
}

class Army::Impl
{
public:
  std::string base;
  std::string destination;
  int strength;

  VariantMap options;
};

Army::Army( EmpirePtr empire )
  : MovableObject( empire ), __INIT_IMPL(Army)
{
  __D_REF(d,Army)

  _animation().load( "world_army" );
  d.strength = 0;
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
  __D_REF(d,Army)

  ObjectPtr obj;
  if( !d.destination.empty() )
  {
    obj = empire()->findObject( d.destination );
  }
  else
  {
    ObjectList objs = empire()->findObjects( location(), config::army::viewRange );
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
    Logger::warning( "!!!Army: nof found object with name " +(d.destination.empty() ? "NULL" : d.destination) );
  }

  MovableObject::_reachedWay();
}

void Army::save(VariantMap& stream) const
{
  MovableObject::save( stream );

  __D_IMPL_CONST(d,Army)
  VARIANT_SAVE_STR_D ( stream, d, base  )
  VARIANT_SAVE_STR_D ( stream, d, destination )
  VARIANT_SAVE_ENUM_D( stream, d, strength )
}

void Army::load(const VariantMap& stream)
{
  MovableObject::load( stream );

  __D_IMPL(d,Army)
  d->options = stream;

  VARIANT_LOAD_STR_D( d, base, stream )
  VARIANT_LOAD_STR_D( d, destination, stream )
  VARIANT_LOAD_ANY_D( d, strength, stream )
}

std::string Army::type() const { return CAESARIA_STR_EXT(Army); }

void Army::setBase(CityPtr base){  _dfunc()->base = base.isValid() ? base->name() : "";  }

void Army::attack(ObjectPtr obj)
{
  __D_REF(d,Army)
  CityPtr baseCity = empire()->findCity( d.base );
  if( baseCity.isValid() && obj.isValid() )
  {
    d.destination = obj->name();
    _findWay( baseCity->location(), obj->location() );

    if( _way().empty() )
    {
      Logger::warning( "Army: cannot find way from {} to {}", d.base, obj->name() );
    }

    attach();
  }
  else
  {
    Logger::warning( "Army: base is " + ( d.base.empty() ? "null" : d.base ) );
    Logger::warning( "Army: object for attack is " + ( obj.isNull() ? "null" : obj->name() ) );
  }
}

void Army::setStrength(int value)
{
  __D_REF(d,Army)
  d.strength = value;
  if( d.strength <= 0 )
    deleteLater();
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
    Army::BattleResult result = Army::battle( otherArmy->strength(), strength() );

    otherArmy->killSoldiers( result.atcLoss );
    killSoldiers( result.defLoss );
  }
}

std::string Army::target() const { return _dfunc()->destination; }

Army::BattleResult Army::battle(unsigned int attackers, unsigned int defenders )
{
  int delimArmy2self = math::percentage( attackers, defenders );
  BattleResult result;

  if( delimArmy2self < 25 )
  {
    result.atcLoss = maxLoss;
    result.defLoss = math::random( 10 );
  }
  else if( delimArmy2self <= 100 )
  {
    int minAtLoss = maxLoss - delimArmy2self;
    int randomAtLoss = math::random(maxLoss+delimArmy2self);
    result.atcLoss = math::clamp<int>( randomAtLoss, minAtLoss, maxLoss );

    int minSelfLoss = math::random( result.atcLoss );
    int randomSelfLoss = math::random( result.atcLoss + delimArmy2self );
    result.defLoss = math::clamp<int>( randomSelfLoss, minSelfLoss, maxLoss );
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

     result.atcLoss = math::clamp<int>( math::random( maxLoss ), 0, minb );
     result.defLoss = math::clamp<int>( math::random( maxLoss ), maxLoss - minb, maxLoss );
  }
  else
  {
    result.atcLoss = math::random( 10 );
    result.defLoss = maxLoss;
  }

  return result;
}

bool Army::_isAgressiveArmy(ArmyPtr) const
{
  return true;
}

}//end namespace world
