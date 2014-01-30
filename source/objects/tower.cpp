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
#include "city/helper.hpp"
#include "gfx/tilemap.hpp"
#include "fortification.hpp"
#include "core/direction.hpp"
#include "walker/wallguard.hpp"
#include "pathway/pathway_helper.hpp"

using namespace constants;

class Tower::Impl
{
public:
  static const int maxPatrolRange = 14;
  typedef std::set< Pathway > PatrolWays;
  PatrolWays patrolWays;
  unsigned int areaHash;
  Point offset;
  bool needResetWays;

  void mayPatroling( const Tile* tile, bool& ret )
  {
    FortificationPtr f = ptr_cast<Fortification>( tile->getOverlay() );
    ret = ( f.isValid() && f->mayPatrol() );
  }
};

Tower::Tower() : ServiceBuilding( Service::guard, building::tower, Size( 2 ) ), _d( new Impl )
{
  setMaxWorkers( 6 );
  setPicture( ResourceGroup::land2a, 149 );
  _fireIncrement = 0;
  _damageIncrement = 0;
}

void Tower::save(VariantMap& stream) const
{
  Building::save( stream );
}

void Tower::load(const VariantMap& stream)
{
  Building::load( stream );
}

bool Tower::canBuild(PlayerCityPtr city, TilePos pos, const TilesArray& ) const
{
  Tilemap& tmap = city->getTilemap();

  bool freeMap[ countDirection ] = { 0 };
  freeMap[ noneDirection ] = tmap.at( pos ).getFlag( Tile::isConstructible );
  freeMap[ north ] = tmap.at( pos + TilePos( 0, 1 ) ).getFlag( Tile::isConstructible );
  freeMap[ east ] = tmap.at( pos + TilePos( 1, 0 ) ).getFlag( Tile::isConstructible );
  freeMap[ northEast ] = tmap.at( pos + TilePos( 1, 1 ) ).getFlag( Tile::isConstructible );

  bool frtMap[ countDirection ] = { 0 };
  frtMap[ noneDirection ] = is_kind_of<Fortification>( tmap.at( pos ).getOverlay() );
  frtMap[ north ] = is_kind_of<Fortification>( tmap.at( pos + TilePos( 0, 1 ) ).getOverlay() );
  frtMap[ northEast ] = is_kind_of<Fortification>( tmap.at( pos + TilePos( 1, 1 ) ).getOverlay() );
  frtMap[ east  ] = is_kind_of<Fortification>( tmap.at( pos + TilePos( 1, 0 ) ).getOverlay() );

  bool mayConstruct = ((frtMap[ noneDirection ] || freeMap[ noneDirection ]) &&
                       (frtMap[ north ] || freeMap[ north ]) &&
                       (frtMap[ east ] || freeMap[ east ]) &&
                       (frtMap[ northEast ] || freeMap[ northEast ]) );

  if( !mayConstruct )
  {
    const_cast<Tower*>( this )->_setError( "##tower_may_build_on_thick_walls##" );
  }

  return mayConstruct;
}

void Tower::_rebuildWays()
{
  _d->patrolWays.clear();
  TilesArray enter = getEnterArea();

  if( enter.empty() )
    return;

  for( int range = Impl::maxPatrolRange; range > 0; range-- )
  {
    TilePos offset( range, range );
    TilesArray tiles = _getCity()->getTilemap().getRectangle( getTilePos() - offset,
                                                              getTilePos() + offset );
    foreach( tile, tiles )
    {
      bool patrolingWall;
      _d->mayPatroling( *tile, patrolingWall );
      if( patrolingWall )
      {
        TilePos tpos = enter.front()->pos();
        Pathway pathway = PathwayHelper::create( tpos, (*tile)->pos(), makeDelegate( _d.data(), &Impl::mayPatroling ) );

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

  if( getWorkersCount() > 0 && !_d->patrolWays.empty() && getWalkers().empty() )
  {
    Impl::PatrolWays::iterator it = _d->patrolWays.begin();
    std::advance( it, rand() % _d->patrolWays.size() );

    WallGuardPtr guard = WallGuard::create( _getCity(), walker::romeGuard );
    guard->send2city( this, *it );

    if( !guard->isDeleted() )
    {
      addWalker( guard.object() );
    }
  }
}

TilesArray Tower::getEnterArea() const
{
  CityHelper helper( _getCity() );
  TilesArray tiles = helper.getAroundTiles( const_cast< Tower* >( this )  );

  for( TilesArray::iterator it=tiles.begin(); it != tiles.end(); )
  {
    FortificationPtr wall = ptr_cast<Fortification>( (*it)->getOverlay() );
    if( wall.isValid() && wall->isTowerEnter() ) { it++; }
    else { it = tiles.erase( it ); }
  }

  return tiles;
}

void Tower::resetPatroling()
{
  _d->needResetWays = true;
}

PathwayList Tower::getWays(TilePos start, FortificationList dest)
{
  PathwayList ret;
  foreach( wall, dest )
  {
    Pathway tmp = PathwayHelper::create( start, (*wall)->getTilePos(), makeDelegate( _d.data(), &Impl::mayPatroling ) );
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
