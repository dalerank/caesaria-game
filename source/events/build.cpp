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
// Copyright 2012-2015 Dalerank, dalerankn8@gmail.com

#include "build.hpp"
#include <GameApp>
#include <GameCore>
#include <GameEvents>
#include <GameObjects>
#include <GameLogger>

#include "walker/enemysoldier.hpp"
#include "city/statistic.hpp"

using namespace gfx;
using namespace city;

namespace events
{

GameEventPtr BuildAny::create( const TilePos& pos, const object::Type type )
{
  return create( pos, Overlay::create( type ) );
}

GameEventPtr BuildAny::create(const TilePos& pos, OverlayPtr overlay)
{
  BuildAny* ev = new BuildAny();
  ev->_pos = pos;
  ev->_overlay = overlay;

  GameEventPtr ret( ev );
  ret->drop();

  return ret;
}

bool BuildAny::_mayExec(Game&, unsigned int) const {  return true;}

void BuildAny::_exec( Game& game, unsigned int )
{  
  if( _overlay.isNull() )
    return;

  OverlayPtr overlay2build = game.city()->getOverlay( _pos );

  bool mayBuild = true;
  if( overlay2build.isValid() )
  {
    mayBuild = overlay2build->isDestructible();
  }

  TilePos offset(10, 10);
  int enemies_n =  game.city()->statistic().walkers.count<EnemySoldier>( _pos - offset, _pos + offset );
  if( enemies_n > 0 && _overlay->group() != object::group::disaster)
  {
    events::dispatch<WarningMessage>( "##too_close_to_enemy_troops##", 2 );
    return;
  }

  if( !_overlay->isDeleted() && mayBuild )
  {
    city::AreaInfo info( game.city(), _pos );
    bool buildOk = _overlay->build( info );

    if( !buildOk )
    {
      Logger::info( "BuildAny: some error when build {0}{1} type:{2}", _pos.i(), _pos.j(), _overlay->name() );
      return;
    }

    Desirability::update( game.city(), _overlay, Desirability::on );
    game.city()->addOverlay( _overlay );

    ConstructionPtr construction = _overlay.as<Construction>();
    if( construction.isValid() )
    {
      auto info = _overlay->info();
      game.city()->treasury().resolveIssue( econ::Issue( econ::Issue::buildConstruction, -info.cost() ) );

      if( construction->group() != object::group::disaster )
      {
        events::dispatch<PlaySound>( "buildok", 1, 100 );
      }

      if( construction->isNeedRoad() && construction->roadside().empty() )
      {
        events::dispatch<WarningMessage>( "##building_need_road_access##", 1 );
      }

      std::string error = construction->errorDesc();
      if( !error.empty() )
      {
        events::dispatch<WarningMessage>( error, 1 );
      }

      WorkingBuildingPtr wb = construction.as<WorkingBuilding>();
      if( wb.isValid() && wb->maximumWorkers() > 0 )
      {
        unsigned int worklessCount = game.city()->statistic().workers.workless();
        if( worklessCount < wb->maximumWorkers() )
        {
          events::dispatch<WarningMessage>( "##city_need_more_workers##", 2 );
        }

        int laborAccessKoeff = wb->laborAccessPercent();
        if( laborAccessKoeff < 50 )
        {
          events::dispatch<WarningMessage>( "##working_build_poor_labor_warning##", 2 );
        }
      }
    }
  }
  else
  {
    auto construction = _overlay.as<Construction>();
    if( construction.isValid() )
    {
      events::dispatch<WarningMessage>( construction->errorDesc(), 1 );
    }
  }

  if(game.isPaused())
  {
    events::dispatch<Step>(Step::once);
  }
}

} //end namespace events
