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

#include "debug_handler.hpp"
#include "gui/contextmenuitem.hpp"
#include "core/logger.hpp"
#include "religion/pantheon.hpp"
#include "city/city.hpp"
#include "city/funds.hpp"
#include "events/random_animals.hpp"
#include "walker/enemysoldier.hpp"
#include "walker/walkers_factory.hpp"
#include "gui/win_mission_window.hpp"
#include "gui/environment.hpp"
#include "city/victoryconditions.hpp"
#include "world/empire.hpp"
#include "world/romechastenerarmy.hpp"
#include "world/barbarian.hpp"
#include "core/saveadapter.hpp"
#include "game/settings.hpp"
#include "events/postpone.hpp"
#include "gfx/layer.hpp"
#include "sound/engine.hpp"
#include "objects/fort.hpp"
#include "world/goodcaravan.hpp"

using namespace constants;
using namespace gfx;

enum {
  add_enemy_archers=0,
  add_enemy_soldiers,
  add_empire_barbarian,
  add_chastener_soldiers,
  add_wolves,
  send_mars_wrath,
  win_mission,
  fail_mission,
  add_1000_dn,
  add_player_money,
  send_chastener,
  test_request,
  send_player_army,
  screenshot,
  send_venus_wrath,
  all_sound_off,
  toggle_grid_visibility,
  toggle_overlay_base,
  toggle_show_path,
  toggle_show_roads,
  toggle_show_object_area,
  toggle_show_walkable_tiles,
  toggle_show_locked_tiles,
  add_soldiers_in_fort,
  toggle_show_flat_tiles,
  send_barbarian_to_player,
  comply_rome_request
};

class DebugHandler::Impl
{
public:
  Game* game;

  void handleEvent( int );
  EnemySoldierPtr makeEnemy( walker::Type type );

public signals:
  Signal2<scene::Level*, bool> failedMissionSignal;
  Signal2<scene::Level*, bool> winMissionSignal;
};

DebugHandler &DebugHandler::instance()
{
  static DebugHandler inst;
  return inst;
}

void DebugHandler::insertTo( Game* game, gui::MainMenu *menu)
{
  _d->game = game;

  gui::ContextMenuItem* tmp = menu->addItem( "Debug", -1, true, true, false, false );
  gui::ContextMenu* debugMenu = tmp->addSubMenu();

#define ADD_DEBUG_EVENT(ev) debugMenu->addItem( #ev, ev );
  ADD_DEBUG_EVENT( add_enemy_archers )
  ADD_DEBUG_EVENT( add_enemy_soldiers );
  ADD_DEBUG_EVENT( add_chastener_soldiers );
  ADD_DEBUG_EVENT( comply_rome_request );
  ADD_DEBUG_EVENT( add_wolves );
  ADD_DEBUG_EVENT( send_mars_wrath );
  ADD_DEBUG_EVENT( add_1000_dn );
  ADD_DEBUG_EVENT( add_player_money );
  ADD_DEBUG_EVENT( send_chastener );
  ADD_DEBUG_EVENT( test_request );
  ADD_DEBUG_EVENT( send_player_army );
  ADD_DEBUG_EVENT( screenshot );
  ADD_DEBUG_EVENT( add_empire_barbarian );
  ADD_DEBUG_EVENT( send_venus_wrath );
  ADD_DEBUG_EVENT( win_mission );
  ADD_DEBUG_EVENT( all_sound_off );
  ADD_DEBUG_EVENT( toggle_grid_visibility );
  ADD_DEBUG_EVENT( toggle_overlay_base );
  ADD_DEBUG_EVENT( toggle_show_path );
  ADD_DEBUG_EVENT( toggle_show_roads );
  ADD_DEBUG_EVENT( toggle_show_object_area );
  ADD_DEBUG_EVENT( add_soldiers_in_fort );
  ADD_DEBUG_EVENT( send_barbarian_to_player );
  ADD_DEBUG_EVENT( fail_mission );
  ADD_DEBUG_EVENT( toggle_show_walkable_tiles );
  ADD_DEBUG_EVENT( toggle_show_locked_tiles );
  ADD_DEBUG_EVENT( toggle_show_flat_tiles );

  CONNECT( debugMenu, onItemAction(), _d.data(), Impl::handleEvent );
#undef ADD_DEBUG_EVENT
}

DebugHandler::~DebugHandler()
{

}

EnemySoldierPtr DebugHandler::Impl::makeEnemy( walker::Type type )
{
  WalkerPtr wlk = WalkerManager::instance().create( type, game->city() );
  EnemySoldierPtr enemy = ptr_cast<EnemySoldier>( wlk );
  if( enemy.isValid() )
  {
    enemy->send2City( game->city()->borderInfo().roadEntry );
  }

  return enemy;
}

Signal2<scene::Level*,bool>& DebugHandler::onFailedMission() { return _d->failedMissionSignal; }
Signal2<scene::Level*,bool>& DebugHandler::onWinMission() { return _d->winMissionSignal; }

DebugHandler::DebugHandler() : _d(new Impl)
{

}

void DebugHandler::Impl::handleEvent(int event)
{
  switch( event )
  {
  case send_mars_wrath:
    religion::rome::Pantheon::mars()->updateRelation( -101.f, game->city() );
  break;

  case add_1000_dn:
    game->city()->funds().resolveIssue(FundIssue(city::Funds::donation, 1000));
  break;

  case add_wolves:
  {
    events::GameEventPtr e = events::RandomAnimals::create( walker::wolf, 10 );
    e->dispatch();
  }
  break;

  case add_enemy_archers: makeEnemy( walker::etruscanArcher ); break;
  case add_enemy_soldiers: makeEnemy( walker::etruscanSoldier ); break;
  case add_chastener_soldiers: makeEnemy( walker::romeChastenerSoldier ); break;

  case comply_rome_request:
  {
    world::GoodCaravanPtr caravan = world::GoodCaravan::create( ptr_cast<world::City>( game->city() ) );
    GoodStock stock( (Good::Type)math::random( Good::goodCount), 1000, 1000 );
    caravan->store().store( stock, stock.qty() );
    caravan->sendTo( game->empire()->rome() );
  }
  break;

  case add_player_money:    game->player()->appendMoney( 1000 );  break;
  case win_mission:
  case fail_mission:
  {
    scene::Level* l = safety_cast<scene::Level*>( game->scene() );
    if( l )
    {
      Signal2<scene::Level*,bool>& signal = event == win_mission ? winMissionSignal : failedMissionSignal;
      emit signal( l, true);
    }
  }
  break;

  case send_chastener:
  {
    world::CityPtr rome = game->empire()->rome();
    PlayerCityPtr plCity = game->city();

    world::RomeChastenerArmyPtr army = world::RomeChastenerArmy::create( game->empire() );
    army->setBase( rome );
    army->attack( ptr_cast<world::Object>( plCity ) );
  }
  break;

  case add_empire_barbarian:
  case send_barbarian_to_player:
  {
    world::BarbarianPtr brb = world::Barbarian::create( game->empire(), Point( 1000, 0 ) );

    if( event == send_barbarian_to_player )
    {
      brb->attack( ptr_cast<world::Object>( game->city() ) );
    }
    else
    {
      brb->attach();
    }
  }
  break;

  case test_request:
  {
    VariantMap rqvm = SaveAdapter::load( ":/test_request.model" );
    events::GameEventPtr e = events::PostponeEvent::create( "", rqvm );
    e->dispatch();
  }
  break;

  case send_venus_wrath:
    religion::rome::Pantheon::venus()->updateRelation( -101.f, game->city() );
  break;

  case all_sound_off:
    audio::Engine::instance().setVolume( audio::ambientSound, 0 );
    audio::Engine::instance().setVolume( audio::themeSound, 0 );
    audio::Engine::instance().setVolume( audio::gameSound, 0 );
  break;

  case toggle_grid_visibility: LayerDrawOptions::instance().toggle( LayerDrawOptions::drawGrid );  break;
  case toggle_overlay_base: LayerDrawOptions::instance().toggle( LayerDrawOptions::shadowOverlay );  break;
  case toggle_show_path: LayerDrawOptions::instance().toggle( LayerDrawOptions::showPath );  break;
  case toggle_show_roads: LayerDrawOptions::instance().toggle( LayerDrawOptions::showRoads );  break;
  case toggle_show_object_area: LayerDrawOptions::instance().toggle( LayerDrawOptions::showObjectArea );  break;
  case toggle_show_walkable_tiles: LayerDrawOptions::instance().toggle( LayerDrawOptions::showWalkableTiles );  break;
  case toggle_show_locked_tiles: LayerDrawOptions::instance().toggle( LayerDrawOptions::showLockedTiles );  break;
  case toggle_show_flat_tiles: LayerDrawOptions::instance().toggle( LayerDrawOptions::showFlatTiles );  break;

  case add_soldiers_in_fort:
  {
    FortList forts;
    forts << game->city()->overlays();

    foreach( it, forts )
    {
      (*it)->setTraineeValue( walker::soldier, 100 );
    }
  }
  break;
  }
}
