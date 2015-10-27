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

#include "mugger.hpp"
#include "objects/house.hpp"
#include "pathway/path_finding.hpp"
#include "constants.hpp"
#include "city/statistic.hpp"
#include "objects/house_level.hpp"
#include "objects/constants.hpp"
#include "core/foreach.hpp"
#include "pathway/astarpathfinding.hpp"
#include "gfx/tilemap.hpp"
#include "objects/constants.hpp"
#include "pathway/pathway_helper.hpp"
#include "corpse.hpp"
#include "ability.hpp"
#include "game/resourcegroup.hpp"
#include "core/variant_map.hpp"
#include "game/gamedate.hpp"
#include "walkers_factory.hpp"

using namespace gfx;

REGISTER_CLASS_IN_WALKERFACTORY(walker::mugger, Mugger)

class Mugger::Impl
{
public:
  typedef enum { searchHouse=0, go2destination,
                 robHouse, go2anyplace, gooutFromCity, wait } State;
  State state;
};

Mugger::Mugger(PlayerCityPtr city)
  : Human( city ), _d( new Impl )
{    
  _setType( walker::mugger );

  addAbility( Illness::create( 0.3, 4) );
}

void Mugger::_reachedPathway()
{
  Walker::_reachedPathway();

  switch( _d->state )
  {
  case Impl::go2destination:
    _animation().clear();
    _animation().load( ResourceGroup::citizen2, 455, 8 );
    _animation().load( ResourceGroup::citizen2, 462, 8, Animation::reverse );
    _setAction( acWork );
    _d->state = Impl::robHouse;
  break;

  default: break;
  }
}

void Mugger::_updateThoughts()
{
  StringArray ret;
  ret << "##rioter_say_1##" << "##rioter_say_2##" << "##rioter_say_3##";

  setThinks( ret.random() );
}

void Mugger::timeStep(const unsigned long time)
{
  Walker::timeStep( time );

  switch( _d->state )
  {
  case Impl::searchHouse:
  {
    TilePos offset(10, 10);

    HouseList houses = _city()->statistic().objects.find<House>( object::house, pos() - offset, pos() + offset );

    std::map< int, HouseList > houseEpxens;
    for( auto house : houses )
    {
      int money = house->getServiceValue( Service::forum );
      houseEpxens[ money ] << house;
    }

    for( auto expHList = houseEpxens.rbegin();
         expHList != houseEpxens.rend(); ++expHList )
    {
      HouseList& hlist = expHList->second;

      for( auto house : hlist )
      {
        Pathway pathway = PathwayHelper::create( pos(), house, PathwayHelper::allTerrain );

        //find path to most expensive house, fire this!!!
        if( pathway.isValid() )
        {
          setPos( pathway.startPos() );
          setPathway( pathway );
          go();
          _d->state = Impl::go2destination;
          break;
        }
      }

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

  case Impl::robHouse:
  {
    if( game::Date::isDayChanged() )
    {
      HouseList houses = _city()->statistic().objects.find<House>( object::house,
                                                                   pos() - TilePos( 1, 1),
                                                                   pos() + TilePos( 1, 1) );

      for( auto house : houses )
      {
        int money = house->getServiceValue( Service::forum );
        if( money > 1 )
        {
          house->appendServiceValue( Service::forum, -1.f );
          break;
        }
      }

      _animation().clear();
      _setAction( acMove );
      _d->state = Impl::searchHouse;
    }
  }
  break;

  default: break;
  }
}

Mugger::~Mugger() {}

void Mugger::send2City( HousePtr house )
{
  TilesArray tiles = house->enterArea();
  if( tiles.empty() )
    return;

  setPos( tiles.random()->pos() );
  _d->state = Impl::searchHouse;
  attach();
}

bool Mugger::die()
{
  bool created = Walker::die();

  if( !created )
  {
    Corpse::create( _city(), pos(), ResourceGroup::citizen2, 447, 454 );
    return true;
  }

  return created;
}

void Mugger::save(VariantMap& stream) const
{
  Walker::save( stream );

  VARIANT_SAVE_ENUM_D( stream, _d, state )
}

void Mugger::load(const VariantMap& stream)
{
  Walker::load( stream );

  VARIANT_LOAD_ENUM_D( _d, state, stream )
}

int Mugger::agressive() const { return 1; }
