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
// Copyright 2012-2014 dalerank, dalerankn8@gmail.com

#include "rioter.hpp"
#include "objects/house.hpp"
#include "pathway/path_finding.hpp"
#include "constants.hpp"
#include "city/helper.hpp"
#include "objects/house_level.hpp"
#include "objects/constants.hpp"
#include "core/foreach.hpp"
#include "pathway/astarpathfinding.hpp"
#include "gfx/tilemap.hpp"
#include "objects/constants.hpp"
#include "pathway/pathway_helper.hpp"
#include "corpse.hpp"
#include "ability.hpp"
#include "core/variant_map.hpp"
#include "events/disaster.hpp"
#include "game/resourcegroup.hpp"
#include "core/variant.hpp"
#include "game/gamedate.hpp"
#include "walkers_factory.hpp"

using namespace constants;
using namespace gfx;

REGISTER_CLASS_IN_WALKERFACTORY(walker::rioter, Rioter)

class Rioter::Impl
{
public:
  typedef enum { searchHouse=0, go2destination, searchAnyBuilding,
                 destroyConstruction, go2anyplace, gooutFromCity, wait } State;
  int houseLevel;
  State state;
  std::set<TileOverlay::Group> excludeGroups;

public:
  Pathway findTarget( PlayerCityPtr city, ConstructionList constructions, TilePos pos );
};

Rioter::Rioter(PlayerCityPtr city) : Human( city ), _d( new Impl )
{    
  _setType( walker::rioter );

  addAbility( Illness::create( 0.3, 4) );
  excludeAttack( objects::disasterGroup );
}

void Rioter::_reachedPathway()
{
  Walker::_reachedPathway();

  switch( _d->state )
  {
  case Impl::go2destination:
    _animationRef().clear();
    _animationRef().load( ResourceGroup::citizen2, 455, 8 );
    _animationRef().load( ResourceGroup::citizen2, 462, 8, Animation::reverse );
    _setAction( acFight );
    _d->state = Impl::destroyConstruction;
  break;

  default: break;
  }
}

void Rioter::_updateThoughts()
{
  StringArray ret;
  ret << "##rioter_say_1##" << "##rioter_say_2##" << "##rioter_say_3##";

  setThinks( ret.random() );
}

void Rioter::timeStep(const unsigned long time)
{
  Walker::timeStep( time );

  switch( _d->state )
  {
  case Impl::searchHouse:
  {
    city::Helper helper( _city() );
    ConstructionList constructions = helper.find<Construction>( objects::house );
    for( ConstructionList::iterator it=constructions.begin(); it != constructions.end(); )
    {
      HousePtr h = ptr_cast<House>( *it );
      if( h->spec().level() <= _d->houseLevel ) { it=constructions.erase( it ); }
      else { ++it; }
    }

    Pathway pathway = _d->findTarget( _city(), constructions, pos() );
    //find more expensive house, fire this!!!
    if( pathway.isValid() )
    {
      setPos( pathway.startPos() );
      setPathway( pathway );
      go();
      _d->state = Impl::go2destination;
    }
    else //not find house, try find any, that rioter can destroy
    {
      _d->state = Impl::searchAnyBuilding;
    }
  }
  break;

  case Impl::searchAnyBuilding:
  {
    city::Helper helper( _city() );
    ConstructionList constructions = helper.find<Construction>( objects::house );

    for( ConstructionList::iterator it=constructions.begin(); it != constructions.end(); )
    {
      TileOverlay::Type type = (*it)->type();
      TileOverlay::Group group = (*it)->group();
      if( type == objects::house || type == objects::road
          || _d->excludeGroups.count( group ) > 0 ) { it=constructions.erase( it ); }
      else { it++; }
    }

    Pathway pathway = _d->findTarget( _city(), constructions, pos() );
    if( pathway.isValid() )
    {
      setPos( pathway.startPos() );
      setPathway( pathway );
      go();
      _d->state = Impl::go2destination;
    }
    else
    {
      _d->state = Impl::go2anyplace;
    }
  }
  break;

  case Impl::go2anyplace:
  {
    Pathway pathway = PathwayHelper::randomWay( _city(), pos(), 10 );

    if( pathway.isValid() )
    {
      setPathway( pathway );
      go();
      _d->state = Impl::go2destination;
    }
    else
    {
      die();
      _d->state = Impl::wait;
    }
  }
  break;

  case Impl::go2destination:
  case Impl::wait:
  break;

  case Impl::destroyConstruction:
  {
    if( game::Date::isDayChanged() )
    {
      city::Helper helper( _city() );
      ConstructionList constructions = helper.find<Construction>( objects::any, pos() - TilePos( 1, 1), pos() + TilePos( 1, 1) );

      for( ConstructionList::iterator it=constructions.begin(); it != constructions.end(); )
      {
        if( (*it)->type() == objects::road || _d->excludeGroups.count( (*it)->group() ) > 0  )
        { it=constructions.erase( it ); }
        else { ++it; }
      }

       if( constructions.empty() )
      {
        _animationRef().clear();
        _setAction( acMove );
        _d->state = Impl::searchHouse;
      }
      else
      {
        foreach( it, constructions )
        {
          ConstructionPtr c = *it;
          c->updateState( Construction::fire, 1 );
          c->updateState( Construction::damage, 1 );
          if( c->state( Construction::damage ) < 10 || c->state( Construction::fire ) < 10 )
          {
            events::GameEventPtr e = events::Disaster::create( c->tile(), events::Disaster::riots );
            e->dispatch();
          }
          break;
        }
      }
    }
  }
  break;

  default: break;
  }
}

RioterPtr Rioter::create(PlayerCityPtr city )
{ 
  RioterPtr ret( new Rioter( city ) );
  ret->drop();
  return ret;
}

Rioter::~Rioter() {}

void Rioter::send2City( BuildingPtr bld )
{
  TilesArray tiles = bld->enterArea();
  if( tiles.empty() )
    return;

  setPos( tiles.random()->pos() );
  _d->houseLevel = 0;

  if( is_kind_of<House>( bld ) )
  {
    HousePtr house = ptr_cast<House>( bld );
    _d->houseLevel = house->spec().level();
  }

  _d->state = Impl::searchHouse;

  if( !isDeleted() )
  {
    _city()->addWalker( WalkerPtr( this ));
  }
}

bool Rioter::die()
{
  bool created = Walker::die();

  if( !created )
  {
    Corpse::create( _city(), pos(), ResourceGroup::citizen2, 447, 454 );
    return true;
  }

  return created;
}

void Rioter::save(VariantMap& stream) const
{
  Walker::save( stream );

  stream[ "houseLevel" ] = _d->houseLevel;
  stream[ "state" ] = (int)_d->state;
}

void Rioter::load(const VariantMap& stream)
{
  Walker::load( stream );

  _d->houseLevel = stream.get( "houseLevel" );
  _d->state = (Impl::State)stream.get( "state" ).toInt();
}

int Rioter::agressive() const { return 1; }
void Rioter::excludeAttack(objects::Group group) { _d->excludeGroups.insert( group ); }

Pathway Rioter::Impl::findTarget(PlayerCityPtr city, ConstructionList constructions, TilePos pos )
{  
  if( !constructions.empty() )
  {
    Pathway pathway;
    for( int i=0; i<10; i++)
    {
      ConstructionList::iterator it = constructions.begin();
      std::advance( it, rand() % constructions.size() );

      pathway = PathwayHelper::create( pos, *it, PathwayHelper::allTerrain );
      if( pathway.isValid() )
      {
        return pathway;
      }
    }
  }

  return Pathway();
}


RioterPtr NativeRioter::create(PlayerCityPtr city)
{
  RioterPtr ret( new NativeRioter( city ) );
  ret->drop();

  return ret;
}

NativeRioter::NativeRioter(PlayerCityPtr city)
  : Rioter( city )
{
  _setType( walker::indigeneRioter );
  excludeAttack( objects::nativeGroup );
}
