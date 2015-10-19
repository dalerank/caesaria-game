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

#include "tower.hpp"
#include "constants.hpp"
#include "game/resourcegroup.hpp"
#include "city/statistic.hpp"
#include "gfx/tilemap.hpp"
#include "fortification.hpp"
#include "core/direction.hpp"
#include "walker/wallguard.hpp"
#include "core/logger.hpp"
#include "pathway/pathway_helper.hpp"
#include "walker/trainee.hpp"
#include "walker/balista.hpp"
#include "objects_factory.hpp"

using namespace gfx;

REGISTER_CLASS_IN_OVERLAYFACTORY(object::tower, Tower)

class Tower::Impl
{
public:
  static const int maxPatrolRange = 14;
  typedef std::set< Pathway > PatrolWays;
  PatrolWays patrolWays;
  unsigned int areaHash;
  bool noEntry;
  Point offset;
  BalistaPtr catapult;
  bool needResetWays;

  void mayPatroling( const Tile* tile, bool& ret )
  {
    FortificationPtr f = tile->overlay<Fortification>();
    ret = ( f.isValid() && f->mayPatrol() );
  }
};

Tower::Tower()
  : ServiceBuilding( Service::guard, object::tower, Size( 2 ) ), _d( new Impl )
{
  _d->noEntry = false;
  setMaximumWorkers( 6 );
  _picture().load( ResourceGroup::land2a, 149 );

  setState( pr::inflammability, 0 );
  setState( pr::collapsibility, 0 );
}

void Tower::save(VariantMap& stream) const
{
  Building::save( stream );
}

void Tower::load(const VariantMap& stream)
{
  Building::load( stream );
}

bool Tower::canBuild(const city::AreaInfo& areaInfo) const
{
  Tilemap& tmap = areaInfo.city->tilemap();

  bool freeMap[ direction::count] = { 0 };
  const TilePos& pos = areaInfo.pos;
  freeMap[ direction::none ] = tmap.at( pos ).getFlag( Tile::isConstructible );
  freeMap[ direction::north ] = tmap.at( pos + TilePos( 0, 1 ) ).getFlag( Tile::isConstructible );
  freeMap[ direction::east ] = tmap.at( pos + TilePos( 1, 0 ) ).getFlag( Tile::isConstructible );
  freeMap[ direction::northEast ] = tmap.at( pos + TilePos( 1, 1 ) ).getFlag( Tile::isConstructible );

  bool frtMap[ direction::count ] = { 0 };
  frtMap[ direction::none ] = is_kind_of<Fortification>( tmap.at( pos ).overlay() );
  frtMap[ direction::north ] = is_kind_of<Fortification>( tmap.at( pos + TilePos( 0, 1 ) ).overlay() );
  frtMap[ direction::northEast ] = is_kind_of<Fortification>( tmap.at( pos + TilePos( 1, 1 ) ).overlay() );
  frtMap[ direction::east  ] = is_kind_of<Fortification>( tmap.at( pos + TilePos( 1, 0 ) ).overlay() );

  bool mayConstruct = ((frtMap[ direction::none ] || freeMap[ direction::none ]) &&
                       (frtMap[ direction::north ] || freeMap[ direction::north ]) &&
                       (frtMap[ direction::east ] || freeMap[ direction::east ]) &&
                       (frtMap[ direction::northEast ] || freeMap[ direction::northEast ]) );

  if( !mayConstruct )
  {
    const_cast<Tower*>( this )->_setError( "##tower_may_build_on_thick_walls##" );
  }

  return mayConstruct;
}

std::string Tower::workersProblemDesc() const
{
  if( _d->patrolWays.empty() )
  {
    return "##tower_need_wall_for_patrol##";
  }

  if( productivity() > 50 )
  {
    if( traineeValue( walker::soldier ) == 0 )
      return "##tower_have_workers_no_soldiers##";
  }

  return ServiceBuilding::workersProblemDesc();
}

void Tower::_rebuildWays()
{
  _d->patrolWays.clear();
  TilesArray enter = enterArea();

  if( enter.empty() )
    return;

  for( int range = Impl::maxPatrolRange; range > 0; range-- )
  {
    TilePos offset( range, range );
    TilesArray tiles = _map().rect( pos() - offset, pos() + offset );
    for( auto tile : tiles )
    {
      bool patrolingWall;
      _d->mayPatroling( tile, patrolingWall );
      if( patrolingWall )
      {
        TilePos tpos = enter.front()->pos();
        Pathway pathway = PathwayHelper::create( tpos, tile->pos(), makeDelegate( _d.data(), &Impl::mayPatroling ) );

        if( pathway.isValid() )
        {
          _d->patrolWays.insert( pathway );
        }
      }
    }
  }

  const int maxWayNumber = 5;
  if( _d->patrolWays.size() > maxWayNumber )
  {
    Impl::PatrolWays::iterator it = _d->patrolWays.begin();
    std::advance( it, _d->patrolWays.size() - maxWayNumber );
    _d->patrolWays.erase( _d->patrolWays.begin(), it );
  }
}

void Tower::deliverService()
{
  if( _d->needResetWays )
  {
    _d->patrolWays.clear();
    _d->needResetWays = false;
  }

  if( _d->patrolWays.empty() )
  {
    _rebuildWays();
  }

  int trValue = traineeValue( walker::soldier );

  if( numberWorkers() > 0
      && !_d->patrolWays.empty()
      && walkers().empty()
      && trValue > 0 )
  {
    Impl::PatrolWays::iterator it = _d->patrolWays.begin();
    std::advance( it, rand() % _d->patrolWays.size() );

    auto guard = WallGuard::create( _city(), walker::romeGuard );
    guard->send2city( this, *it );

    addWalker( guard.object() );
  }

  if( trValue > 1 )
  {
    if( _d->catapult.isNull() )
    {
      _d->catapult = Balista::create( _city() );
      _d->catapult->setPos( pos()+TilePos( 1, 0 ) );
      _d->catapult->setBase( this );
    }
  }

  if( _d->catapult.isValid() )
  {
    _d->catapult->setActive( trValue > 1 );
  }
}

void Tower::burn()
{
  Logger::warning( "WARNING: Tower cant be burn. Ignore." );
}

TilesArray Tower::enterArea() const
{
  TilesArray tiles = _city()->statistic().map.around( this );

  for( auto it=tiles.begin(); it != tiles.end(); )
  {
    auto wall = (*it)->overlay<Fortification>();
    if( wall.isValid() && wall->isTowerEnter() ) { ++it; }
    else { it = tiles.erase( it ); }
  }

  return tiles;
}

void Tower::resetPatroling() {  _d->needResetWays = true; }

Point Tower::offset(const Tile& tile, const Point& subpos) const
{
  return Point( 0, -65 );
}

PathwayList Tower::getWays(TilePos start, FortificationList dest)
{
  PathwayList ret;
  for( auto wall : dest )
  {
    Pathway tmp = PathwayHelper::create( start, wall->pos(), makeDelegate( _d.data(), &Impl::mayPatroling ) );
    if( tmp.isValid() )
    {    
      ret.push_back( PathwayPtr( new Pathway( tmp ) ) );
      ret.back()->drop();
    }
  }

  return ret;
}

Pathway Tower::getWay(TilePos start, TilePos stop)
{
  return PathwayHelper::create( start, stop, makeDelegate( _d.data(), &Impl::mayPatroling ) );
}

float Tower::evaluateTrainee(walker::Type traineeType)
{
  int limiter = ( traineeType == walker::soldier ) ? 4 : 1;
  float value = ServiceBuilding::evaluateTrainee( traineeType );

  return (value / limiter);
}
