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
#include "good/goodstore_simple.hpp"
#include "game/resourcegroup.hpp"
#include "core/logger.hpp"
#include "merchant.hpp"
#include "gfx/animation.hpp"
#include "city.hpp"
#include "core/variant_map.hpp"
#include "game/gamedate.hpp"
#include "events/notification.hpp"

namespace world
{

namespace {
static const Point barbarianStartLocation( 1500, 0 );
}

class Barbarian::Impl
{
public:  
  typedef enum { findAny, go2object, hunting, goaway } Mode;
  Mode mode;
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

std::string Barbarian::type() const { return CAESARIA_STR_EXT(Barbarian); }

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

int Barbarian::viewDistance() const { return 60; }

bool Barbarian::_isAgressiveArmy(ArmyPtr other) const
{
  return !is_kind_of<Barbarian>( other );
}

void Barbarian::_check4attack()
{
  MovableObjectList mobjects;
  mobjects << empire()->objects();
  mobjects.remove( this );

  std::map< int, MovableObjectPtr > distanceMap;

  foreach( it, mobjects )
  {
    float distance = location().distanceTo( (*it)->location() );    
    distanceMap[ (int)distance ] = *it;
  }

  foreach( it, distanceMap )
  {
    if( it->first < 20 )
    {
      _attackObject( ptr_cast<Object>( it->second ) );
      break;
    }
    else if( it->first < viewDistance() )
    {
      bool validWay = _findWay( location(), it->second->location() );
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
     foreach( it, cities )
     {
       float distance = location().distanceTo( (*it)->location() );
       int month2lastAttack = math::clamp<int>( 12 - (*it)->lastAttack().monthsTo( currentDate ), 0, 12 );
       citymap[ month2lastAttack * 100 + (int)distance ] = *it;
     }

     foreach( it, citymap )
     {
       bool validWay = _findWay( location(), it->second->location() );
       if( validWay )
       {
         _d->mode = Impl::go2object;

         events::GameEventPtr e = events::Notification::attack( it->second->name(), "##barbaria_attack_empire_city##", this );
         e->dispatch();

         break;
       }
     }
  }
}

void Barbarian::_goaway()
{
  bool validWay = _findWay( location(), barbarianStartLocation );
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
  ObjectList objs = empire()->findObjects( location(), 20 );
  objs.remove( this );

  bool successAttack = false;
  foreach( i, objs )
  {
    successAttack = _attackObject( *i );
    if( successAttack )
      break;
  }

  if( successAttack )
    _goaway();
}

bool Barbarian::_attackObject(ObjectPtr obj)
{
  if( is_kind_of<Merchant>( obj ) )
  {
    obj->deleteLater();
    return true;
  }
  else if( is_kind_of<City>( obj ) )
  {
    CityPtr pcity = ptr_cast<City>( obj );

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
  setSpeed( 4.f );

  _animation().clear();
  _animation().load( ResourceGroup::empirebits, 53, 16 );
  Size size = _animation().frame( 0 ).size();
  _animation().setOffset( Point( -size.width() / 2, size.height() / 2 ) );
  _animation().setLoop( gfx::Animation::loopAnimation );
}


}
