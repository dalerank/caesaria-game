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
#include "city.hpp"

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
    else if( it->first < 200 )
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

     foreach( it, cities )
     {
       float distance = location().distanceTo( (*it)->location() );
       citymap[ (int)distance ] = *it;
     }

     foreach( it, citymap )
     {
       bool validWay = _findWay( location(), it->second->location() );
       if( validWay )
       {
         _d->mode = Impl::go2object;
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

void Barbarian::_reachedWay()
{
  if( _d->mode == Impl::go2object )
  {
    ObjectList objs = empire()->findObjects( location(), 20 );
    objs.remove( this );

    bool successAttack = _attackObject( objs );
    if( successAttack )
    {
      _goaway();
    }
  }
  else if( _d->mode == Impl::goaway )
  {
    deleteLater();
  }
}

bool Barbarian::_attackObject(ObjectList objs)
{
  foreach( i, objs )
  {
    if( _attackObject( *i ) )
      return true;
  }

  return false;
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
    obj->addObject( this );
    return true;
  }

  return false;
}

Barbarian::Barbarian( EmpirePtr empire )
 : MovableObject( empire ), _d( new Impl )
{
  _d->mode = Impl::findAny;
  setSpeed( 4.f );

  setPicture( gfx::Picture::load( ResourceGroup::empirebits, 54 ) );
}


}
