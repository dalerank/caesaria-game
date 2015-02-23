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

#include "ruins.hpp"
#include "game/resourcegroup.hpp"
#include "core/position.hpp"
#include "walker/serviceman.hpp"
#include "gfx/tile.hpp"
#include "gfx/tilemap.hpp"
#include "city/helper.hpp"
#include "events/build.hpp"
#include "constants.hpp"
#include "core/foreach.hpp"
#include "core/variant_map.hpp"
#include "game/gamedate.hpp"
#include "walker/dustcloud.hpp"
#include "city/cityservice_fire.hpp"
#include "objects_factory.hpp"
#include "gfx/animation_bank.hpp"

using namespace constants;
using namespace gfx;

REGISTER_CLASS_IN_OVERLAYFACTORY(object::burned_ruins, BurnedRuins)
REGISTER_CLASS_IN_OVERLAYFACTORY(object::burning_ruins, BurningRuins)
REGISTER_CLASS_IN_OVERLAYFACTORY(object::collapsed_ruins, CollapsedRuins)
REGISTER_CLASS_IN_OVERLAYFACTORY(object::plague_ruins, PlagueRuins)

BurningRuins::BurningRuins()
  : Ruins( object::burning_ruins )
{
  setState( pr::fire, 99 );
  setState( pr::inflammability, 0 );
  setState( pr::collapsibility, 0 );

  setPicture( ResourceGroup::land2a, 187 );
  _animationRef() = AnimationBank::instance().simple( AnimationBank::animFire+2 );
  //_animationRef().load( ResourceGroup::land2a, 188, 8 );
  //_animationRef().setOffset( Point( 14, 26 ) );
  _fgPicturesRef().resize(1);
  //_animationRef().setDelay( math::random( 6 ) );
}

void BurningRuins::timeStep(const unsigned long time)
{
  Building::timeStep(time);

  _animationRef().update( time );
  _fgPicturesRef().back() = _animationRef().currentFrame();

  if( game::Date::isDayChanged() )
  {
    TilePos offset( 2, 2 );
    city::Helper helper( _city() );
    BuildingList buildings = helper.find<Building>( object::any, pos() - offset, pos() + offset );

    foreach( it, buildings)
    {
      if( (*it)->group() != object::group::disaster )
      {
        (*it)->updateState( pr::fire, _value );
      }
    }

    if( state( pr::fire ) > 0 )
    {
      updateState( pr::fire, -1 );
      if( state( pr::fire ) == 50 )
      {
        setPicture( ResourceGroup::land2a, 214 );
        _animationRef() = AnimationBank::instance().simple( AnimationBank::animFire + 1 );
        //_animationRef().clear();
        //_animationRef().load( ResourceGroup::land2a, 215, 8);
        //_animationRef().setOffset( Point( 14, 26 ) );
      }
      else if( state( pr::fire ) == 25 )
      {
        setPicture( ResourceGroup::land2a, 223 );
        _animationRef() = AnimationBank::instance().simple( AnimationBank::animFire + 0 );
        //_animationRef().clear();
        //_animationRef().load(ResourceGroup::land2a, 224, 8);
        //_animationRef().setOffset( Point( 14, 18 ) );
      }
    }
    else
    {
      deleteLater();
      _animationRef().clear();
      _fgPicturesRef().clear();
    }
  }

  if( game::Date::isWeekChanged() )
  {
    _animationRef().setDelay( math::random( 2 )+1 );
  }
}

void BurningRuins::destroy()
{
  Building::destroy();

  BurnedRuinsPtr p( new BurnedRuins() );
  p->drop();
  p->setInfo( info() );

  city::FirePtr fire;
  fire << _city()->findService( city::Fire::defaultName() );

  if( fire.isValid() )
  {
    fire->rmLocation( pos() );
  }

  events::GameEventPtr event = events::BuildAny::create( pos(), p.object() );
  event->dispatch();
}

void BurningRuins::collapse() {}

void BurningRuins::burn(){}

bool BurningRuins::build( const city::AreaInfo& info)
{
  Building::build( info );
  //while burning can't remove it
  tile().setFlag( Tile::tlTree, false );
  tile().setFlag( Tile::tlRoad, false );

  city::FirePtr fire;
  fire << info.city->findService( city::Fire::defaultName() );
  _value = (info.city->getOption( PlayerCity::fireKoeff ) / 100.f) *
           (defaultForce / 100.f);

  if( fire.isValid() )
  {
    fire->addLocation( info.pos );
  }

  return true;
}   

bool BurningRuins::isWalkable() const{  return (state( pr::fire ) == 0);}
bool BurningRuins::isDestructible() const{  return isWalkable();}
bool BurningRuins::canDestroy() const { return (state( pr::fire ) == 0); }

float BurningRuins::evaluateService( ServiceWalkerPtr walker )
{
  if ( Service::prefect == walker->serviceType() )
  {
    return state( pr::fire );
  }

  return 0;
}

void BurningRuins::applyService(ServiceWalkerPtr walker)
{
  if ( Service::prefect == walker->serviceType() )
  {
    double delta =  walker->serviceValue() / 2;
    updateState( pr::fire, -delta );
  }
}

bool BurningRuins::isNeedRoadAccess() const{  return false; }
void BurnedRuins::timeStep( const unsigned long ){}

BurnedRuins::BurnedRuins() : Ruins( object::burned_ruins )
{
  setPicture( ResourceGroup::land2a, 111 + rand() % 8 );
}

bool BurnedRuins::build( const city::AreaInfo& info )
{
  Building::build( info );

  tile().setFlag( Tile::tlRock, false );
  return true;
}

bool BurnedRuins::isWalkable() const{  return true; }
bool BurnedRuins::isFlat() const{ return true;}
bool BurnedRuins::isNeedRoadAccess() const{  return false;}
void BurnedRuins::destroy(){ Building::destroy();}

CollapsedRuins::CollapsedRuins() : Ruins(object::collapsed_ruins)
{
  setState( pr::damage, 1 );
  setState( pr::inflammability, 0 );
  setState( pr::collapsibility, 0 );

  _animationRef().load( ResourceGroup::sprites, 1, 8 );
  _animationRef().setOffset( Point( 14, 26 ) );
  _animationRef().setDelay( 4 );
  _animationRef().setLoop( false );
  _fgPicturesRef().resize(1);
}

void CollapsedRuins::burn() {}

bool CollapsedRuins::build( const city::AreaInfo& info )
{
  Building::build( info );

  tile().setFlag( Tile::tlTree, false );
  tile().setFlag( Tile::tlRoad, false );
  setPicture( ResourceGroup::land2a, 111 + math::random( 8 ) );

  if( !_alsoBuilt )
  {
    DustCloud::create( info.city, info.pos, 4 );
  }

  return true;
}

void CollapsedRuins::collapse() {}
bool CollapsedRuins::isWalkable() const{  return true;}
bool CollapsedRuins::isFlat() const {return true;}
bool CollapsedRuins::isNeedRoadAccess() const{  return false;}

PlagueRuins::PlagueRuins() : Ruins( object::plague_ruins )
{
  setState( pr::fire, 99 );
  setState( pr::collapsibility, 0 );

  setPicture( ResourceGroup::land2a, 187 );
  _animationRef() = AnimationBank::instance().simple( AnimationBank::animFire + 2 );

  //_animationRef().load( ResourceGroup::land2a, 188, 8 );
  //_animationRef().setOffset( Point( 14, 26 ) );
  _fgPicturesRef().resize(2);
  _fgPicturesRef()[ 1 ] = Picture::load( ResourceGroup::sprites, 218 );
  _fgPicturesRef()[ 1 ].setOffset( Point( 20, 35 ) );
}

void PlagueRuins::timeStep(const unsigned long time)
{
  _animationRef().update( time );
  _fgPicturesRef()[ 0 ] = _animationRef().currentFrame();

  if( game::Date::isDayChanged() )
  {
    if( state( pr::fire ) > 0 )
    {
      updateState( pr::fire, -1 );
      if( state( pr::fire ) == 50 )
      {
        setPicture( ResourceGroup::land2a, 214 );
        _animationRef() = AnimationBank::instance().simple( AnimationBank::animFire + 1 );

        //_animationRef().clear();
        //_animationRef().load( ResourceGroup::land2a, 215, 8);
        //_animationRef().setOffset( Point( 14, 26 ) );
      }
      else if( state( pr::fire ) == 25 )
      {
        setPicture( ResourceGroup::land2a, 223 );
        _animationRef() = AnimationBank::instance().simple( AnimationBank::animFire + 0 );

        //_animationRef().clear();
        //_animationRef().load(ResourceGroup::land2a, 224, 8);
        //_animationRef().setOffset( Point( 14, 18 ) );
      }
    }
    else
    {
      deleteLater();
      _animationRef().clear();
      _fgPicturesRef().clear();
    }
  }
}

void PlagueRuins::destroy()
{
  Building::destroy();

  BurnedRuinsPtr p( new BurnedRuins() );
  p->drop();
  p->setInfo( info() );

  events::GameEventPtr event = events::BuildAny::create( pos(), p.object() );
  event->dispatch();
}

void PlagueRuins::applyService(ServiceWalkerPtr walker){}
void PlagueRuins::burn(){}
bool PlagueRuins::isDestructible() const { return isWalkable(); }

bool PlagueRuins::build( const city::AreaInfo& info )
{
  Building::build( info );
  //while burning can't remove it
  tile().setFlag( Tile::tlTree, false );
  tile().setFlag( Tile::tlRoad, false );
  tile().setFlag( Tile::tlRock, true );

  return true;
}

bool PlagueRuins::isWalkable() const{  return (state( pr::fire ) == 0);}
bool PlagueRuins::isNeedRoadAccess() const{  return false;}

Ruins::Ruins(object::Type type)
  : Building( type, Size(1) ), _alsoBuilt( true )
{

}

void Ruins::save(VariantMap& stream) const
{
  Building::save( stream );

  stream[ "text" ] = Variant( _parent );
}

void Ruins::load(const VariantMap& stream)
{
  Building::load( stream );

  _parent = stream.get( "text" ).toString();
}
