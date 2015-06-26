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

#include "debug_handler.hpp"
#include "gui/contextmenuitem.hpp"
#include "core/logger.hpp"
#include "religion/pantheon.hpp"
#include "city/statistic.hpp"
#include "game/funds.hpp"
#include "game/player.hpp"
#include "events/random_animals.hpp"
#include "walker/enemysoldier.hpp"
#include "walker/walkers_factory.hpp"
#include "gui/win_mission_window.hpp"
#include "gui/environment.hpp"
#include "city/victoryconditions.hpp"
#include "world/empire.hpp"
#include "city/cityservice_festival.hpp"
#include "world/romechastenerarmy.hpp"
#include "world/barbarian.hpp"
#include "core/saveadapter.hpp"
#include "game/settings.hpp"
#include "walker/romesoldier.hpp"
#include "events/postpone.hpp"
#include "layers/layer.hpp"
#include "sound/engine.hpp"
#include "vfs/directory.hpp"
#include "objects/fort.hpp"
#include "events/dispatcher.hpp"
#include "gui/loadfiledialog.hpp"
#include "gfx/tilemap.hpp"
#include "good/helper.hpp"
#include "good/store.hpp"
#include "world/goodcaravan.hpp"
#include "events/earthquake.hpp"
#include "events/random_fire.hpp"
#include "events/random_damage.hpp"
#include "events/changeemperor.hpp"
#include "events/random_plague.hpp"
#include "events/scribemessage.hpp"
#include "world/emperor.hpp"
#include "objects/warehouse.hpp"
#include "vfs/archive.hpp"
#include "vfs/filesystem.hpp"
#include "game/resourceloader.hpp"
#include "religion/config.hpp"
#include "world/computer_city.hpp"
#include "gui/property_workspace.hpp"

using namespace gfx;
using namespace citylayer;
using namespace gui;

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
  comply_rome_request,
  change_emperor,
  add_city_border,
  earthquake,
  toggle_experimental_options,
  kill_all_enemies,
  send_exporter,
  random_fire,
  random_collapse,
  random_plague,
  reload_aqueducts,
  crash_favor,
  add_scribe_messages,
  send_venus_smallcurse,
  send_mars_spirit,
  run_script,
  show_fest,
  add_favor,
  add_wheat_to_warehouse,
  add_fish_to_warehouse,
  add_olives_to_warehouse,
  add_fruit_to_warehouse,
  add_grape_to_warehouse,
  add_vegetable_to_warehouse,
  add_clay_to_warehouse,
  add_timber_to_warehouse,
  add_iron_to_warehouse,
  add_marble_to_warehouse,
  add_pottery_to_warehouse,
  add_furniture_to_warehouse,
  add_weapons_to_warehouse,
  add_wine_to_warehouse,
  add_oil_to_warehouse,
  remove_favor,
  property_browser
};

class DebugHandler::Impl
{
public:
  Game* game;

  void handleEvent( int );
  EnemySoldierPtr makeEnemy( walker::Type type );
  void addGoods2Wh( good::Product type );
  void runScript(std::string filename);
  gui::ContextMenu* debugMenu;

public signals:
  Signal2<scene::Level*, bool> failedMissionSignal;
  Signal2<scene::Level*, bool> winMissionSignal;
};

void DebugHandler::insertTo( Game* game, gui::MainMenu* menu)
{
  _d->game = game;

  gui::ContextMenuItem* tmp = menu->addItem( "Debug", -1, true, true, false, false );
  _d->debugMenu = tmp->addSubMenu();

#define ADD_DEBUG_EVENT(section, ev) { gui::ContextMenuItem* item = _d->debugMenu->addItem( section, #ev, ev ); \
                                       CONNECT( item, onAction(), _d.data(), Impl::handleEvent ); }

  ADD_DEBUG_EVENT( "enemies", add_enemy_archers )
  ADD_DEBUG_EVENT( "enemies", add_enemy_soldiers )
  ADD_DEBUG_EVENT( "enemies", add_chastener_soldiers )
  ADD_DEBUG_EVENT( "enemies", add_wolves )
  ADD_DEBUG_EVENT( "enemies", send_chastener )
  ADD_DEBUG_EVENT( "enemies", add_empire_barbarian )
  ADD_DEBUG_EVENT( "enemies", send_barbarian_to_player )
  ADD_DEBUG_EVENT( "enemies", kill_all_enemies )

  ADD_DEBUG_EVENT( "request", comply_rome_request )
  ADD_DEBUG_EVENT( "request", test_request )

  ADD_DEBUG_EVENT( "religion", send_mars_wrath )
  ADD_DEBUG_EVENT( "religion", send_mars_spirit )
  ADD_DEBUG_EVENT( "religion", send_venus_wrath )
  ADD_DEBUG_EVENT( "religion", send_venus_smallcurse )

  ADD_DEBUG_EVENT( "money", add_1000_dn )
  ADD_DEBUG_EVENT( "money", add_player_money )

  ADD_DEBUG_EVENT( "goods", add_wheat_to_warehouse )
  ADD_DEBUG_EVENT( "goods", add_fish_to_warehouse )
  ADD_DEBUG_EVENT( "goods", add_olives_to_warehouse )
  ADD_DEBUG_EVENT( "goods", add_fruit_to_warehouse )
  ADD_DEBUG_EVENT( "goods", add_grape_to_warehouse )
  ADD_DEBUG_EVENT( "goods", add_vegetable_to_warehouse )
  ADD_DEBUG_EVENT( "goods", add_clay_to_warehouse )
  ADD_DEBUG_EVENT( "goods", add_timber_to_warehouse )
  ADD_DEBUG_EVENT( "goods", add_iron_to_warehouse )
  ADD_DEBUG_EVENT( "goods", add_marble_to_warehouse )
  ADD_DEBUG_EVENT( "goods", add_pottery_to_warehouse )
  ADD_DEBUG_EVENT( "goods", add_furniture_to_warehouse )
  ADD_DEBUG_EVENT( "goods", add_weapons_to_warehouse )
  ADD_DEBUG_EVENT( "goods", add_wine_to_warehouse )
  ADD_DEBUG_EVENT( "goods", add_oil_to_warehouse )

  ADD_DEBUG_EVENT( "other", send_player_army )
  ADD_DEBUG_EVENT( "other", screenshot )

  ADD_DEBUG_EVENT( "disaster", random_fire )
  ADD_DEBUG_EVENT( "disaster", random_collapse )
  ADD_DEBUG_EVENT( "disaster", random_plague )
  ADD_DEBUG_EVENT( "disaster", earthquake )

  ADD_DEBUG_EVENT( "game", win_mission )
  ADD_DEBUG_EVENT( "game", fail_mission )
  ADD_DEBUG_EVENT( "game", change_emperor )
  ADD_DEBUG_EVENT( "game", property_browser )

  ADD_DEBUG_EVENT( "city", add_soldiers_in_fort )
  ADD_DEBUG_EVENT( "city", add_city_border )
  ADD_DEBUG_EVENT( "city", send_exporter )
  ADD_DEBUG_EVENT( "city", crash_favor )
  ADD_DEBUG_EVENT( "city", add_scribe_messages )
  ADD_DEBUG_EVENT( "city", run_script )
  ADD_DEBUG_EVENT( "city", show_fest )
  ADD_DEBUG_EVENT( "city", add_favor )
  ADD_DEBUG_EVENT( "city", remove_favor )

  ADD_DEBUG_EVENT( "options", all_sound_off )
  ADD_DEBUG_EVENT( "options", reload_aqueducts )
  ADD_DEBUG_EVENT( "options", toggle_experimental_options )

  ADD_DEBUG_EVENT( "draw", toggle_grid_visibility )
  ADD_DEBUG_EVENT( "draw", toggle_overlay_base )
  ADD_DEBUG_EVENT( "draw", toggle_show_path )
  ADD_DEBUG_EVENT( "draw", toggle_show_roads )
  ADD_DEBUG_EVENT( "draw", toggle_show_object_area )
  ADD_DEBUG_EVENT( "draw", toggle_show_walkable_tiles )
  ADD_DEBUG_EVENT( "draw", toggle_show_locked_tiles )
  ADD_DEBUG_EVENT( "draw", toggle_show_flat_tiles )
#undef ADD_DEBUG_EVENT
}

void DebugHandler::setVisible(bool visible)
{
  if( _d->debugMenu != 0)
    _d->debugMenu->setVisible( visible );
}

DebugHandler::~DebugHandler() {}

EnemySoldierPtr DebugHandler::Impl::makeEnemy( walker::Type type )
{
  EnemySoldierPtr enemy = WalkerManager::instance().create<EnemySoldier>( type, game->city() );
  if( enemy.isValid() )
  {
    enemy->send2City( game->city()->borderInfo().roadEntry );
  }

  return enemy;
}

void DebugHandler::Impl::addGoods2Wh(good::Product type)
{
  WarehouseList whList = city::statistic::getObjects<Warehouse>( game->city(), object::warehouse );
  foreach( wh, whList)
  {
    WarehousePtr warehouse = *wh;
    good::Stock stock(type, 400, 400 );
    warehouse->store().store( stock, 400 );
  }
}

void DebugHandler::Impl::runScript(std::string filename)
{
  events::Dispatcher::instance().load( filename );
}

Signal2<scene::Level*,bool>& DebugHandler::onFailedMission() { return _d->failedMissionSignal; }
Signal2<scene::Level*,bool>& DebugHandler::onWinMission() { return _d->winMissionSignal; }

DebugHandler::DebugHandler() : _d(new Impl)
{
  _d->debugMenu = 0;
}

void DebugHandler::Impl::handleEvent(int event)
{
  switch( event )
  {
  case send_mars_wrath:
    religion::rome::Pantheon::mars()->updateRelation( religion::debug::doWrath, game->city() );
  break;

  case add_1000_dn:
    game->city()->treasury().resolveIssue(econ::Issue(econ::Issue::donation, 1000));
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
    good::Stock stock( good::Helper::random(), 1000, 1000 );
    caravan->store().store( stock, stock.qty() );
    caravan->sendTo( game->empire()->rome() );
  }
  break;

  case property_browser:
  {
    int hash = Hash( CAESARIA_STR_A(PropertyWorkspace) );
    PropertyWorkspace* browser = safety_cast<PropertyWorkspace*>( game->gui()->findWidget( hash ) );
    if( !browser )
    {
      browser = new PropertyWorkspace( game->gui()->rootWidget(), game->scene(), Rect( 0, 0, 500, 700 ) );
      browser->setCity( game->city() );
      game->scene()->installEventHandler( browser->handler() );
    }
  }
  break;

  case send_exporter:
  {
    world::CityList cities = game->empire()->cities();
    foreach( it, cities )
    {
      world::ComputerCityPtr ccity = (*it).as<world::ComputerCity>();
      if( ccity.isValid() )
      {
        ccity->__debugSendMerchant();
      }
    }
  }
  break;

  case add_player_money:    game->player()->appendMoney( 1000 );  break;

  case add_favor:
  case remove_favor:
  {
    std::string cityName = game->city()->name();
    game->empire()->emperor().updateRelation( cityName, event == add_favor ? +10 : -10 );
  }
  break;

  case show_fest:
  {
    city::FestivalPtr fest = city::statistic::getService<city::Festival>( game->city() );
    if( fest.isValid() )
      fest->now();
  }
  break;

  case add_wheat_to_warehouse: addGoods2Wh( good::wheat ); break;
  case add_fish_to_warehouse:  addGoods2Wh( good::fish  ); break;
  case add_olives_to_warehouse: addGoods2Wh( good::olive); break;
  case add_fruit_to_warehouse: addGoods2Wh( good::fruit ); break;
  case add_grape_to_warehouse: addGoods2Wh( good::grape ); break;
  case add_vegetable_to_warehouse:addGoods2Wh( good::vegetable); break;
  case add_clay_to_warehouse:  addGoods2Wh( good::clay  ); break;
  case add_timber_to_warehouse:addGoods2Wh( good::timber); break;
  case add_iron_to_warehouse:  addGoods2Wh( good::iron  ); break;
  case add_marble_to_warehouse:addGoods2Wh( good::marble); break;
  case add_pottery_to_warehouse:addGoods2Wh( good::pottery); break;
  case add_furniture_to_warehouse:addGoods2Wh( good::furniture); break;
  case add_weapons_to_warehouse:addGoods2Wh( good::weapon ); break;
  case add_wine_to_warehouse: addGoods2Wh( good::wine ); break;
  case add_oil_to_warehouse: addGoods2Wh( good::oil ); break;

  case win_mission:
  case fail_mission:
  {
    scene::Level* l = safety_cast<scene::Level*>( game->scene() );
    if( l )
    {
      Signal2<scene::Level*,bool>& signal = (event == win_mission ? winMissionSignal : failedMissionSignal);
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

  case change_emperor:
  {
    events::GameEventPtr e = events::ChangeEmperor::create();
    VariantMap vm = config::load( ":/test_emperor.model" );
    e->load( vm );
    e->dispatch();
  }
  break;

  case kill_all_enemies:
  {
     EnemySoldierList enemies = city::statistic::getWalkers<EnemySoldier>( game->city(), walker::any, gfx::tilemap::invalidLocation() );

     foreach( it, enemies )
       (*it)->die();
  }
  break;

  case add_city_border:   {    game->city()->tilemap().addSvkBorder();  }  break;

  case toggle_experimental_options:
  {
    bool enable = SETTINGS_VALUE( experimental );
    SETTINGS_SET_VALUE( experimental, !enable );
  }
  break;

  case add_scribe_messages:
  {
    events::GameEventPtr e = events::ScribeMessage::create( "test_message", "this is test message from yout scribes" );
    e->dispatch();
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

  case crash_favor:
    game->empire()->emperor().updateRelation( game->city()->name(), -100 );
  break;

  case random_fire:
  {
    events::GameEventPtr e = events::RandomFire::create();
    e->dispatch();
  }
  break;

  case random_plague:
  {
    events::GameEventPtr e = events::RandomPlague::create();
    e->dispatch();
  }
  break;

  case random_collapse:
  {
    events::GameEventPtr e = events::RandomDamage::create();
    e->dispatch();
  }
  break;

  case earthquake:
  {
    int mapsize = game->city()->tilemap().size();
    TilePos start( math::random(mapsize), math::random(mapsize) );
    TilePos stop( math::random(mapsize), math::random(mapsize) );
    events::GameEventPtr e = events::EarthQuake::create( start, stop );
    e->dispatch();
  }
  break;

  case reload_aqueducts:
  {
     vfs::ArchivePtr archive = vfs::FileSystem::instance().mountArchive( ":/gfx/pics_aqueducts.zip" );

     if( archive.isNull() )
     {
       return;
     }

     ResourceLoader rc;
     rc.loadFiles( archive );
     vfs::FileSystem::instance().unmountArchive(archive);
  }
  break;

  case test_request:
  {
    VariantMap rqvm = config::load( ":/test_request.model" );
    events::GameEventPtr e = events::PostponeEvent::create( "", rqvm );
    e->dispatch();
  }
  break;

  case send_venus_wrath:
    religion::rome::Pantheon::venus()->updateRelation( religion::debug::doWrath, game->city() );
  break;

  case send_venus_smallcurse:
    religion::rome::Pantheon::venus()->updateRelation( religion::debug::doSmallCurse, game->city() );
  break;

  case send_mars_spirit:
    religion::rome::Pantheon::mars()->updateRelation( religion::debug::doBlessing, game->city() );
  break;

  case all_sound_off:
    audio::Engine::instance().setVolume( audio::ambient, 0 );
    audio::Engine::instance().setVolume( audio::theme, 0 );
    audio::Engine::instance().setVolume( audio::game, 0 );
  break;

  case run_script:
  {
    Widget* parent = game->gui()->rootWidget();
    dialog::LoadFile* wnd = dialog::LoadFile::create( parent,
                                                      Rect(),
                                                      vfs::Path( ":/scripts/" ), ".model",
                                                      -1 );
    wnd->setCenter( parent->center() );

    CONNECT( wnd, onSelectFile(), this, Impl::runScript );
    wnd->setTitle( "Select file" );
    wnd->setText( "open" );
  }
  break;

  case toggle_grid_visibility: DrawOptions::instance().toggle( DrawOptions::drawGrid );  break;
  case toggle_overlay_base: DrawOptions::instance().toggle( DrawOptions::shadowOverlay );  break;
  case toggle_show_path: DrawOptions::instance().toggle( DrawOptions::showPath );  break;
  case toggle_show_roads: DrawOptions::instance().toggle( DrawOptions::showRoads );  break;
  case toggle_show_object_area: DrawOptions::instance().toggle( DrawOptions::showObjectArea );  break;
  case toggle_show_walkable_tiles: DrawOptions::instance().toggle( DrawOptions::showWalkableTiles );  break;
  case toggle_show_locked_tiles: DrawOptions::instance().toggle( DrawOptions::showLockedTiles );  break;
  case toggle_show_flat_tiles: DrawOptions::instance().toggle( DrawOptions::showFlatTiles );  break;

  case add_soldiers_in_fort:
  {
    FortList forts = city::statistic::getObjects<Fort>( game->city() );

    foreach( it, forts )
    {
      int howMuchAdd = 16 - (*it)->walkers().size();
      TilesArray tiles = (*it)->enterArea();
      for( int i=0; i < howMuchAdd; i++ )
      {
        RomeSoldierPtr soldier = RomeSoldier::create( game->city(), walker::legionary );
        soldier->send2city( *it, tiles.front()->pos() );
        (*it)->addWalker( soldier.object() );
      }
    }
  }
  break;
  }
}
