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

#include "barbarian.hpp"
#include "empire.hpp"
#include "good/storage.hpp"
#include "game/resourcegroup.hpp"
#include "core/logger.hpp"
#include "merchant.hpp"
#include "gfx/animation.hpp"
#include "city.hpp"
#include "core/variant_map.hpp"
#include "game/gamedate.hpp"
#include "events/notification.hpp"
#include "city/states.hpp"
#include "config.hpp"
#include "objects_factory.hpp"

namespace world
{

REGISTER_CLASS_IN_WORLDFACTORY(Barbarian)

const Point Barbarian::startLocation = Point( 1500, 0 );

class Barbarian::Impl
{
public:  
  typedef enum { findAny, go2object, hunting, goaway } Mode;
  Mode mode;
  int minPop4attack;
  VariantMap options;
};

BarbarianPtr Barbarian::create(EmpirePtr empire, Point location)
{
  BarbarianPtr ret( new Barbarian( empire ) );
  ret->setLocation( location );
  ret->setStrength( 10 + math::random( 50 ) );
  ret->_check4attack();  
  ret->drop();

  return ret;
}

std::string Barbarian::about(Object::AboutType type)
{
  std::string ret;
  switch(type)
  {
  case empireMap: ret = "##enemy_army_threating_a_city##";      break;
  default:        ret = "##enemy_army_unknown_about##";  break;
  }

  return ret;
}

std::string Barbarian::type() const { return TEXT(Barbarian); }

void Barbarian::timeStep(unsigned int time)
{
  MovableObject::timeStep( time );
}

void Barbarian::save(VariantMap& stream) const
{
  MovableObject::save( stream );
}

void Barbarian::load(const VariantMap& stream)
{
  MovableObject::load( stream );

  _d->options = stream;
}

void Barbarian::updateStrength(int value)
{
  setStrength( strength() + value );
}

int Barbarian::searchRange() const { return config::barbarian::viewRange; }
void Barbarian::setMinpop4attack(int value) { _d->minPop4attack = value; }

bool Barbarian::_isAgressiveArmy(ArmyPtr other) const
{
  return !is_kind_of<Barbarian>( other );
}

void Barbarian::_check4attack()
{
  auto movables = empire()->objects().select<MovableObject>();
  movables.remove( this );

  std::map< int, MovableObjectPtr > distanceMap;

  for( auto obj : movables )
  {
    float distance = location().distanceTo( obj->location() );
    distanceMap[ (int)distance ] = obj;
  }

  for( auto& item : distanceMap )
  {
    if( item.first < config::barbarian::attackRange )
    {
      _attackObject( item.second.as<Object>() );
      break;
    }
    else if( item.first < searchRange() )
    {
      bool validWay = _findWay( location(), item.second->location() );
      if( validWay )
      {
        _d->mode = Impl::go2object;
        break;
      }
    }
  }

  if( _way().empty() )
  {
     CityList cities = empire()->cities();
     std::map< int, CityPtr > citymap;

     DateTime currentDate = game::Date::current();

     for( auto city : cities )
     {
       if( city->states().population < (unsigned int)_d->minPop4attack )
         continue;

       float distance = location().distanceTo( city->location() );
       int month2lastAttack = math::clamp<int>( DateTime::monthsInYear - city->lastAttack().monthsTo( currentDate ), 0, DateTime::monthsInYear );
       citymap[ month2lastAttack * 100 + (int)distance ] = city;
     }

     for( auto& item : citymap )
     {
       bool validWay = _findWay( location(), item.second->location() );
       if( validWay )
       {
         _d->mode = Impl::go2object;

         events::GameEventPtr e = events::Notify::attack( item.second->name(), "##barbaria_attack_empire_city##", this );
         e->dispatch();

         break;
       }
     }
  }
}

void Barbarian::_goaway()
{
  bool validWay = _findWay( location(), startLocation );
  if( !validWay )
  {
    Logger::warning( "Barbarian: cant find way for out" );
    deleteLater();
  }

  _d->mode = Impl::goaway;
}

void Barbarian::_noWay()
{
   _attackAny();
}

void Barbarian::_reachedWay()
{
  if( _d->mode == Impl::go2object )
  {
    _attackAny();
  }
  else if( _d->mode == Impl::goaway )
  {
    deleteLater();
  }
}

void Barbarian::_attackAny()
{
  ObjectList objs = empire()->findObjects( location(), config::barbarian::attackRange );
  objs.remove( this );

  bool successAttack = false;
  for( auto object : objs )
  {
    successAttack = _attackObject( object );
    if( successAttack )
      break;
  }

  if( successAttack )
    _goaway();
}

bool Barbarian::_attackObject(ObjectPtr obj)
{
  if( obj.is<Merchant>() )
  {
    obj->deleteLater();
    return true;
  }
  else if( obj.is<City>() )
  {
    CityPtr pcity = obj.as<City>();
    pcity->addObject( this );
    return !pcity->strength();
  }
  //else if( )

  return false;
}

Barbarian::Barbarian( EmpirePtr empire )
 : Army( empire ), _d( new Impl )
{
  _d->mode = Impl::findAny;
  _d->minPop4attack = 1000;
  setSpeed( 4.f );

  _animation().clear();
  _animation().load( "world_barbarian" );
}

}//end namespace world
