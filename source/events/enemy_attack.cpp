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
#include "core/variant_map.hpp"
#include "core/logger.hpp"
#include "walker/enemysoldier.hpp"
#include "city/cityservice_military.hpp"
#include "walker/walkers_factory.hpp"
#include "walker/helper.hpp"
#include "factory.hpp"

using namespace constants;
using namespace gfx;

namespace events
{

namespace {
CAESARIA_LITERALCONST(type)
CAESARIA_LITERALCONST(items)
CAESARIA_LITERALCONST(target)
}

REGISTER_EVENT_IN_FACTORY(EnemyAttack, "enemy_attack" )

class AttackPriorityHelper : public EnumsHelper<EnemySoldier::AttackPriority>
{
public:
  AttackPriorityHelper() : EnumsHelper<EnemySoldier::AttackPriority>( EnemySoldier::attackAll )
  {
    append( EnemySoldier::attackAll, "attack_all" );
    append( EnemySoldier::attackBestBuilding, "best_building" );
    append( EnemySoldier::attackCitizen, "citizen" );
    append( EnemySoldier::attackSenate, "gold" );
    append( EnemySoldier::attackIndustry, "industry" );
    append( EnemySoldier::attackFood, "food" );
  }
};

class EnemyAttack::Impl
{
public:
  VariantMap items;
  EnemySoldier::AttackPriority attackPriority;
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

    std::string soldierType = soldiers.get( lc_type ).toString();
    int soldierNumber = soldiers.get( "count" );

    Variant vCityPop = soldiers.get( "city.pop" );
    if( vCityPop.isValid() )
    {
      soldierNumber = game.city()->population() * vCityPop.toFloat();
    }

    TilePos location( -1, -1 );
    Variant vLocation = soldiers.get( "location" );

    if( vLocation.toString() == "random" )
    {
      Tilemap& tmap = game.city()->tilemap();
      int lastIndex = tmap.size();
      TilesArray tiles = tmap.getRectangle( TilePos( 0, 0), TilePos(lastIndex, lastIndex) );

      tiles = tiles.walkableTiles( true );

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
    for( int k=0; k < soldierNumber; k++ )
    {
      WalkerPtr wlk = WalkerManager::instance().create( wtype, game.city() );
      EnemySoldierPtr enemy = ptr_cast<EnemySoldier>( wlk );
      if( enemy.isValid() )
      {
        enemy->send2City( location );
        enemy->wait( math::random( k * 30 ) );
        enemy->setAttackPriority( _d->attackPriority );
        enemy->setSpeedMultiplier( 0.7 + math::random( 60 ) / 100.f  );
      }           
    }
  }
}

bool EnemyAttack::_mayExec(Game&, unsigned int) const { return true; }
bool EnemyAttack::isDeleted() const { return _dfunc()->isDeleted; }

void EnemyAttack::load(const VariantMap& stream)
{
  __D_IMPL(_d,EnemyAttack)
  _d->items = stream.get( lc_items ).toMap();

  std::string targetStr = stream.get( lc_target ).toString();

  AttackPriorityHelper helper;
  if( targetStr == "random" )
  {
    _d->attackPriority = (EnemySoldier::AttackPriority)math::random( EnemySoldier::attackCount );
  }
  else
  {
    _d->attackPriority = helper.findType( targetStr );
  }
}

VariantMap EnemyAttack::save() const
{
  VariantMap ret;

  __D_IMPL_CONST(_d,EnemyAttack);

  ret[ lc_items ] = _d->items;

  return ret;
}

EnemyAttack::EnemyAttack() : __INIT_IMPL(EnemyAttack)
{
  __D_IMPL(_d,EnemyAttack)
  _d->isDeleted = false;
}

}//end namespace events
