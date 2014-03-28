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

#include "enemy_attack.hpp"
#include "game/game.hpp"
#include "city/helper.hpp"
#include "game/gamedate.hpp"
#include "events/dispatcher.hpp"
#include "gfx/tilemap.hpp"
#include "core/logger.hpp"
#include "walker/enemysoldier.hpp"
#include "walker/walkers_factory.hpp"
#include "walker/helper.hpp"

using namespace constants;

namespace events
{

class EnemyAttack::Impl
{
public:
  VariantMap items;
  VariantMap events;
  bool isDeleted;
};

GameEventPtr EnemyAttack::create()
{
  GameEventPtr ret( new EnemyAttack() );
  ret->drop();

  return ret;
}

void EnemyAttack::_exec( Game& game, unsigned int time)
{
  __D_IMPL(_d,EnemyAttack);

  if( _d->isDeleted )
    return;

  _d->isDeleted = true;
  foreach( i, _d->items )
  {
    VariantMap soldiers = i->second.toMap();

    std::string soldierType = soldiers.get( "type" ).toString();
    int soldierNumber = soldiers.get( "count" );
    TilePos location( -1, -1 );
    Variant vLocation = soldiers.get( "location" );

    if( vLocation.toString() == "random" )
    {
      Tilemap& tmap = game.city()->tilemap();
      int lastIndex = tmap.size();
      TilesArray tiles = tmap.getRectangle( TilePos( 0, 0), TilePos(lastIndex, lastIndex) );

      for( TilesArray::iterator t=tiles.begin(); t != tiles.end(); )
      {
        if( !(*t)->isWalkable( true ) ) { t = tiles.erase( t ); }
        else { ++t; }
      }

      Tile* tile = tiles[ math::random( tiles.size() ) ];
      if( tile )
      {
        location = tile->pos();
      }
    }
    else
    {

    }

    walker::Type wtype = WalkerHelper::getType( soldierType );
    WalkerPtr wlk = WalkerManager::instance().create( wtype, game.city() );
    for( int k=0; k < soldierNumber; k++ )
    {
      EnemySoldierPtr enemy = ptr_cast<EnemySoldier>( wlk );
      if( enemy.isValid() )
      {
        enemy->send2City( location );
      }
    }
  }

  events::Dispatcher::instance().load( _d->events );
}

bool EnemyAttack::_mayExec(Game&, unsigned int) const { return true; }
bool EnemyAttack::isDeleted() const {  __D_IMPL_CONST(_d,EnemyAttack); return _d->isDeleted; }

void EnemyAttack::load(const VariantMap& stream)
{
  __D_IMPL(_d,EnemyAttack)
  _d->events = stream.get( "exec" ).toMap();
  _d->items = stream.get( "items" ).toMap();
}

VariantMap EnemyAttack::save() const
{
  VariantMap ret;

  __D_IMPL_CONST(_d,EnemyAttack);

  ret[ "exec" ] = _d->events;
  ret[ "items" ] = _d->items;

  return ret;
}

EnemyAttack::EnemyAttack() : __INIT_IMPL(EnemyAttack)
{
  __D_IMPL(_d,EnemyAttack)
  _d->isDeleted = false;
}

}//end namespace events
