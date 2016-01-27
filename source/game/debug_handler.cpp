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
#include "city/city.hpp"
#include "objects/tree.hpp"
#include "objects/clay_pit.hpp"
#include "game/player.hpp"
#include "events/random_animals.hpp"
#include "walker/enemysoldier.hpp"
#include "walker/walkers_factory.hpp"
#include "gui/win_mission_window.hpp"
#include "gui/environment.hpp"
#include "city/victoryconditions.hpp"
#include "world/empire.hpp"
#include "core/common.hpp"
#include "world/empire.hpp"
#include "city/cityservice_festival.hpp"
#include "world/romechastenerarmy.hpp"
#include "world/barbarian.hpp"
#include "core/saveadapter.hpp"
#include "objects/granary.hpp"
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
#include "objects/house.hpp"
#include "objects/house_habitants.hpp"
#include "gui/property_workspace.hpp"
#include "objects/factory.hpp"
#include "events/warningmessage.hpp"
#include "sound/themeplayer.hpp"
#include "city/build_options.hpp"
#include "objects/house_spec.hpp"

using namespace gfx;
using namespace citylayer;
using namespace events;
using namespace gui;

enum {
  enemies,
  requests,
  divinity,
  money,
  goods,
  empiremap,
  factories,
  other,
  buildings,
  disaster,
  level,
  in_city,
  options,
  house,
  draw,
  empire
};

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
  send_merchants,
  random_fire,
  random_collapse,
  random_plague,
  reload_aqueducts,
  crash_favor,
  add_scribe_messages,
  send_venus_smallcurse,
  send_neptune_wrath,
  send_mars_spirit,
  run_script,
  show_fest,
  add_favor,
  add_wheat_to_granary,
  add_fish_to_granary,
  add_meat_to_granary,
  add_fruit_to_granary,
  add_vegetable_to_granary,
  add_wheat_to_warehouse,
  add_fish_to_warehouse,
  add_meat_to_warehouse,
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
  property_browser,
  make_generation,
  all_wheatfarms_ready,
  all_wahrf_ready,
  all_olivefarms_ready,
  all_fruitfarms_ready,
  all_grapefarms_ready,
  all_vegetablefarms_ready,
  all_claypit_ready,
  all_timberyard_ready,
  all_ironmin_ready,
  all_marblequarry_ready,
  all_potteryworkshtp_ready,
  all_furnitureworksop_ready,
  all_weaponworkshop_ready,
  all_wineworkshop_ready,
  all_oilworkshop_ready,
  decrease_sentiment,
  increase_sentiment,
  reload_buildings_config,
  toggle_show_buildings,
  toggle_show_trees,
  toggle_show_empireMapTiles,
  toggle_show_rocks,
  toggle_lock_empiremap,
  forest_fire,
  forest_grow,
  increase_max_level,
  decrease_max_level,
  increase_house_level,
  decrease_house_level,
  lock_house_level,
  enable_constructor_mode,
  show_requests,
  show_attacks,
  reset_fire_risk,
  reset_collapse_risk,
  toggle_shipyard_enable,
  toggle_reservoir_enable,
  toggle_wineshop_enable,
  toggle_vinard_enable,
  fill_random_claypit,
  empire_toggle_capua,
  empire_toggle_londinium,
  next_theme
};

class DebugHandler::Impl
{
public:
  Game* game;

  void handleEvent( int );
  EnemySoldierPtr makeEnemy( walker::Type type );
  void setFactoryReady(object::Type type);
  void updateSentiment( int delta );
  void addGoods2Wh( good::Product type );
  void addGoods2Gr( good::Product type );
  void reloadConfigs();
  void runScript(std::string filename);
  void toggleBuildOptions(object::Type type);
  void toggleEmpireCityEnable(const std::string &name);
  gui::ContextMenu* debugMenu;

#ifdef DEBUG
  FileChangeObserver configUpdater;
#endif

  struct {
    Signal2<scene::Level*, bool> failedMission;
    Signal2<scene::Level*, bool> winMission;
  } signal;
};

void DebugHandler::insertTo( Game* game, gui::MainMenu* menu)
{
  _d->game = game;

  gui::ContextMenuItem* tmp = menu->addItem( "Debug", -1, true, true, false, false );
  _d->debugMenu = tmp->addSubMenu();

#define ADD_DEBUG_EVENT(section, ev) { gui::ContextMenuItem* item = _d->debugMenu->addItem( #section, #ev, ev ); \
                                       item->onAction().connect( _d.data(), &Impl::handleEvent ); }

  ADD_DEBUG_EVENT( enemies, add_enemy_archers )
  ADD_DEBUG_EVENT( enemies, add_enemy_soldiers )
  ADD_DEBUG_EVENT( enemies, add_chastener_soldiers )
  ADD_DEBUG_EVENT( enemies, add_wolves )
  ADD_DEBUG_EVENT( enemies, send_chastener )
  ADD_DEBUG_EVENT( enemies, add_empire_barbarian )
  ADD_DEBUG_EVENT( enemies, send_barbarian_to_player )
  ADD_DEBUG_EVENT( enemies, kill_all_enemies )

  ADD_DEBUG_EVENT( requests, comply_rome_request )
  ADD_DEBUG_EVENT( requests, test_request )

  ADD_DEBUG_EVENT( divinity, send_mars_wrath )
  ADD_DEBUG_EVENT( divinity, send_mars_spirit )
  ADD_DEBUG_EVENT( divinity, send_venus_wrath )
  ADD_DEBUG_EVENT( divinity, send_neptune_wrath )
  ADD_DEBUG_EVENT( divinity, send_venus_smallcurse )

  ADD_DEBUG_EVENT( money, add_1000_dn )
  ADD_DEBUG_EVENT( money, add_player_money )

  ADD_DEBUG_EVENT( goods, add_wheat_to_warehouse )
  ADD_DEBUG_EVENT( goods, add_fish_to_warehouse )
  ADD_DEBUG_EVENT( goods, add_meat_to_warehouse )
  ADD_DEBUG_EVENT( goods, add_olives_to_warehouse )
  ADD_DEBUG_EVENT( goods, add_fruit_to_warehouse )
  ADD_DEBUG_EVENT( goods, add_grape_to_warehouse )
  ADD_DEBUG_EVENT( goods, add_vegetable_to_warehouse )
  ADD_DEBUG_EVENT( goods, add_clay_to_warehouse )
  ADD_DEBUG_EVENT( goods, add_timber_to_warehouse )
  ADD_DEBUG_EVENT( goods, add_iron_to_warehouse )
  ADD_DEBUG_EVENT( goods, add_marble_to_warehouse )
  ADD_DEBUG_EVENT( goods, add_pottery_to_warehouse )
  ADD_DEBUG_EVENT( goods, add_furniture_to_warehouse )
  ADD_DEBUG_EVENT( goods, add_weapons_to_warehouse )
  ADD_DEBUG_EVENT( goods, add_wine_to_warehouse )
  ADD_DEBUG_EVENT( goods, add_oil_to_warehouse )

  ADD_DEBUG_EVENT( goods, add_wheat_to_granary )
  ADD_DEBUG_EVENT( goods, add_fish_to_granary )
  ADD_DEBUG_EVENT( goods, add_meat_to_granary )
  ADD_DEBUG_EVENT( goods, add_fruit_to_granary )
  ADD_DEBUG_EVENT( goods, add_vegetable_to_granary )

  ADD_DEBUG_EVENT( factories, all_wheatfarms_ready )
  ADD_DEBUG_EVENT( factories, all_wahrf_ready )
  ADD_DEBUG_EVENT( factories, all_olivefarms_ready )
  ADD_DEBUG_EVENT( factories, all_fruitfarms_ready )
  ADD_DEBUG_EVENT( factories, all_grapefarms_ready )
  ADD_DEBUG_EVENT( factories, all_vegetablefarms_ready )
  ADD_DEBUG_EVENT( factories, all_claypit_ready )
  ADD_DEBUG_EVENT( factories, all_timberyard_ready )
  ADD_DEBUG_EVENT( factories, all_ironmin_ready )
  ADD_DEBUG_EVENT( factories, all_marblequarry_ready )
  ADD_DEBUG_EVENT( factories, all_potteryworkshtp_ready )
  ADD_DEBUG_EVENT( factories, all_furnitureworksop_ready )
  ADD_DEBUG_EVENT( factories, all_weaponworkshop_ready )
  ADD_DEBUG_EVENT( factories, all_wineworkshop_ready )
  ADD_DEBUG_EVENT( factories, all_oilworkshop_ready )

  ADD_DEBUG_EVENT( other, send_player_army )
  ADD_DEBUG_EVENT( other, screenshot )
  ADD_DEBUG_EVENT( other, enable_constructor_mode )
  ADD_DEBUG_EVENT( other, next_theme )

  ADD_DEBUG_EVENT( buildings, toggle_shipyard_enable )
  ADD_DEBUG_EVENT( buildings, toggle_reservoir_enable )
  ADD_DEBUG_EVENT( buildings, toggle_wineshop_enable )
  ADD_DEBUG_EVENT( buildings, toggle_vinard_enable )

  ADD_DEBUG_EVENT( disaster, random_fire )
  ADD_DEBUG_EVENT( disaster, random_collapse )
  ADD_DEBUG_EVENT( disaster, random_plague )
  ADD_DEBUG_EVENT( disaster, earthquake )
  ADD_DEBUG_EVENT( disaster, fill_random_claypit )
  ADD_DEBUG_EVENT( disaster, forest_fire )

  ADD_DEBUG_EVENT( level, win_mission )
  ADD_DEBUG_EVENT( level, fail_mission )
  ADD_DEBUG_EVENT( level, change_emperor )
  ADD_DEBUG_EVENT( level, property_browser )
  ADD_DEBUG_EVENT( level, show_requests )
  ADD_DEBUG_EVENT( level, show_attacks )

  ADD_DEBUG_EVENT( empire, send_merchants )
  ADD_DEBUG_EVENT( empire, toggle_lock_empiremap )
  ADD_DEBUG_EVENT( empire, empire_toggle_capua )
  ADD_DEBUG_EVENT( empire, empire_toggle_londinium )

  ADD_DEBUG_EVENT( in_city, add_soldiers_in_fort )
  ADD_DEBUG_EVENT( in_city, add_city_border )
  ADD_DEBUG_EVENT( in_city, crash_favor )
  ADD_DEBUG_EVENT( in_city, add_scribe_messages )
  ADD_DEBUG_EVENT( in_city, show_fest )
  ADD_DEBUG_EVENT( in_city, add_favor )
  ADD_DEBUG_EVENT( in_city, remove_favor )
  ADD_DEBUG_EVENT( in_city, make_generation )
  ADD_DEBUG_EVENT( in_city, decrease_sentiment )
  ADD_DEBUG_EVENT( in_city, increase_sentiment )
  ADD_DEBUG_EVENT( in_city, forest_grow )
  ADD_DEBUG_EVENT( in_city, reset_fire_risk )
  ADD_DEBUG_EVENT( in_city, reset_collapse_risk )

  ADD_DEBUG_EVENT( house, increase_max_level )
  ADD_DEBUG_EVENT( house, decrease_max_level )
  ADD_DEBUG_EVENT( house, increase_house_level )
  ADD_DEBUG_EVENT( house, decrease_house_level )
  ADD_DEBUG_EVENT( house, lock_house_level )

  ADD_DEBUG_EVENT( options, run_script )
  ADD_DEBUG_EVENT( options, all_sound_off )
  ADD_DEBUG_EVENT( options, reload_aqueducts )
  ADD_DEBUG_EVENT( options, toggle_experimental_options )
  ADD_DEBUG_EVENT( options, reload_buildings_config )

  ADD_DEBUG_EVENT( draw, toggle_grid_visibility )
  ADD_DEBUG_EVENT( draw, toggle_overlay_base )
  ADD_DEBUG_EVENT( draw, toggle_show_path )
  ADD_DEBUG_EVENT( draw, toggle_show_roads )
  ADD_DEBUG_EVENT( draw, toggle_show_buildings )
  ADD_DEBUG_EVENT( draw, toggle_show_trees )
  ADD_DEBUG_EVENT( draw, toggle_show_object_area )
  ADD_DEBUG_EVENT( draw, toggle_show_walkable_tiles )
  ADD_DEBUG_EVENT( draw, toggle_show_locked_tiles )
  ADD_DEBUG_EVENT( draw, toggle_show_flat_tiles )
  ADD_DEBUG_EVENT( draw, toggle_show_rocks )

  ADD_DEBUG_EVENT( empiremap, toggle_show_empireMapTiles )
#undef ADD_DEBUG_EVENT

#ifdef DEBUG
  _d->configUpdater.setFilename( ":/construction.model" );
  CONNECT( &_d->configUpdater, onChange, _d.data(), Impl::reloadConfigs );
#endif
}

void DebugHandler::setVisible(bool visible)
{
  if( _d->debugMenu != 0)
    _d->debugMenu->setVisible( visible );
}

void DebugHandler::Impl::toggleEmpireCityEnable( const std::string& name )
{
  world::CityPtr city = game->empire()->findCity( name );
  std::string text ;

  if( city.isValid() )
  {
    bool enabled = city->isAvailable();
    city->setAvailable( !enabled );
    text = fmt::format( "Empire city {} is now {}", name, !enabled ? "enabled" : "disabled" );
  }
  else
  {
    text = fmt::format( "Empire city {} is not exit", name );
  }
  events::dispatch<WarningMessage>( text, WarningMessage::negative );
}

void DebugHandler::Impl::toggleBuildOptions( object::Type type )
{
  city::development::Options options;
  options = game->city()->buildOptions();
  options.setBuildingAvailable( type, !options.isBuildingAvailable( type ) );
  game->city()->setBuildOptions( options );
}

DebugHandler::~DebugHandler() {}

EnemySoldierPtr DebugHandler::Impl::makeEnemy( walker::Type type )
{
  EnemySoldierPtr enemy = Walker::create<EnemySoldier>( type, game->city() );
  if( enemy.isValid() )
  {
    enemy->send2City( game->city()->getBorderInfo( PlayerCity::roadEntry ).epos() );
  }

  return enemy;
}

void DebugHandler::Impl::addGoods2Wh(good::Product type)
{
  WarehouseList whList = game->city()->statistic().objects.find<Warehouse>();
  for( auto warehouse : whList)
  {
    good::Stock stock(type, 400, 400 );
    warehouse->store().store( stock, 400 );
  }
}

void DebugHandler::Impl::addGoods2Gr(good::Product type)
{
  auto grList = game->city()->statistic().objects.find<Granary>();
  for( auto granary : grList)
  {
    good::Stock stock(type, 400, 400 );
    granary->store().store( stock, 400 );
  }
}


void DebugHandler::Impl::reloadConfigs()
{
  auto overlays = game->city()->overlays();
  for( auto overlay : overlays )
    overlay->reinit();
}

void DebugHandler::Impl::runScript(std::string filename)
{
  events::Dispatcher::instance().load( filename );
}

Signal2<scene::Level*,bool>& DebugHandler::onFailedMission() { return _d->signal.failedMission; }
Signal2<scene::Level*,bool>& DebugHandler::onWinMission() { return _d->signal.winMission; }

DebugHandler::DebugHandler() : _d(new Impl)
{
  _d->debugMenu = 0;
}

void DebugHandler::Impl::setFactoryReady( object::Type type )
{
  auto factories = game->city()->statistic().objects.find<Factory>( type );
  for( auto factory : factories )
  {
    if( factory->numberWorkers() > 0 )
    {
      float progress = factory->progress();
      factory->updateProgress( 101.f - progress );
    }
  }
}

void DebugHandler::Impl::updateSentiment(int delta)
{
  auto houses = game->city()->statistic().houses.find();
  for( auto house : houses )
    house->updateState( pr::happiness, delta );
}

void DebugHandler::Impl::handleEvent(int event)
{
  switch( event )
  {
  case send_mars_wrath:
    religion::rome::Pantheon::mars()->updateRelation( religion::debug::doWrath, game->city() );
  break;

  case send_neptune_wrath:
    religion::rome::Pantheon::neptune()->updateRelation( religion::debug::doWrath, game->city() );
  break;

  case add_1000_dn:
    game->city()->treasury().resolveIssue(econ::Issue(econ::Issue::donation, 1000));
  break;

  case add_wolves:
  {
    events::dispatch<RandomAnimals>( walker::wolf, 10 );
  }
  break;

  case add_enemy_archers: makeEnemy( walker::etruscanArcher ); break;
  case add_enemy_soldiers: makeEnemy( walker::etruscanSoldier ); break;
  case add_chastener_soldiers: makeEnemy( walker::romeChastenerSoldier ); break;

  case comply_rome_request:
  {
    auto caravan = world::GoodCaravan::create( ptr_cast<world::City>( game->city() ) );
    good::Stock stock( good::Helper::random(), 1000, 1000 );
    caravan->store().store( stock, stock.qty() );
    caravan->sendTo( game->empire()->capital() );
  }
  break;

  case toggle_shipyard_enable: toggleBuildOptions( object::shipyard );  break;
  case toggle_reservoir_enable: toggleBuildOptions( object::reservoir );  break;
  case toggle_wineshop_enable: toggleBuildOptions( object::wine_workshop );  break;
  case toggle_vinard_enable: toggleBuildOptions( object::vinard );  break;

  case empire_toggle_capua: toggleEmpireCityEnable( "Capua" ); break;
  case empire_toggle_londinium: toggleEmpireCityEnable( "Londinium" ); break;

  case next_theme:
  {
    auto player = game->city()->statistic().services.find<audio::ThemePlayer>();
    if( player.isValid() )
    {
      player->next();
    }
  }
  break;

  case fill_random_claypit:
  {
    auto pit = game->city()->overlays().select<ClayPit>().random();
    if( pit.isValid() )
      pit->flood();
  }
  break;

  case enable_constructor_mode:
  {
    auto level = safety_cast<scene::Level*>( game->scene() );
    if( level )
    {
      level->setConstructorMode( true );
    }
  }
  break;

  case property_browser:
  {
    int hash = Hash( TEXT(PropertyWorkspace) );
    PropertyWorkspace* browser = safety_cast<PropertyWorkspace*>( game->gui()->findWidget( hash ) );
    if( !browser )
    {
      browser = &game->gui()->add<PropertyWorkspace>( game->scene(), Rect( 0, 0, 500, 700 ) );
      browser->setCity( game->city() );
      game->scene()->installEventHandler( browser->handler() );
    }
  }
  break;

  case reload_buildings_config: reloadConfigs(); break;

  case forest_fire:
  {
    auto forest = game->city()->overlays().select<Tree>().random( 2 );
    for( auto tree : forest )
      tree->burn();
  }
  break;

  case reset_fire_risk:
  {
    BuildingList buildings = game->city()->overlays().select<Building>();
    for( auto building : buildings )
      building->setState( pr::fire, 0 );
  }
  break;

  case reset_collapse_risk:
  {
    BuildingList buildings = game->city()->overlays().select<Building>();
    for( auto building : buildings )
      building->setState( pr::damage, 0 );
  }
  break;

  case send_merchants:
  {
    world::CityList cities = game->empire()->cities();
    for( auto acity : cities )
    {
      auto compCity = acity.as<world::ComputerCity>();
      if( compCity.isValid() )
      {
        compCity->__debugSendMerchant();
      }
    }
  }
  break;

  case increase_max_level:
  case decrease_max_level:
  {
    city::VictoryConditions conditions;
    conditions = game->city()->victoryConditions();
    conditions.setMaxHouseLevel( conditions.maxHouseLevel() + (event == increase_max_level ? 1 : -1) );
    game->city()->setVictoryConditions( conditions );

    std::string levelName = HouseSpecHelper::instance().getSpec( conditions.maxHouseLevel() ).internalName();
    events::dispatch<WarningMessage>( "DEBUG: House max level is " + levelName,
                                      events::WarningMessage::neitral );
  }
  break;

  case increase_house_level:
  case decrease_house_level:
  {
    HouseList houses = game->city()->overlays().select<House>();
    for( auto house : houses )
      house->__debugChangeLevel( event == increase_house_level ? 1 : -1 );
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
    city::FestivalPtr festivals = game->city()->statistic().services.find<city::Festival>();
    if( festivals.isValid() )
      festivals->doFestivalNow();
  }
  break;

  case add_wheat_to_warehouse: addGoods2Wh( good::wheat ); break;
  case add_fish_to_warehouse:  addGoods2Wh( good::fish  ); break;
  case add_meat_to_warehouse:  addGoods2Wh( good::meat  ); break;
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

  case add_wheat_to_granary: addGoods2Gr( good::wheat ); break;
  case add_fish_to_granary: addGoods2Gr( good::fish ); break;
  case add_meat_to_granary: addGoods2Gr( good::meat ); break;
  case add_fruit_to_granary: addGoods2Gr( good::fruit ); break;
  case add_vegetable_to_granary: addGoods2Gr( good::vegetable ); break;

  case all_wheatfarms_ready: setFactoryReady( object::wheat_farm ); break;
  case all_wahrf_ready: setFactoryReady( object::wharf ); break;
  case all_olivefarms_ready: setFactoryReady( object::olive_farm ); break;
  case all_fruitfarms_ready: setFactoryReady( object::fig_farm ); break;
  case all_grapefarms_ready: setFactoryReady( object::vinard ); break;
  case all_vegetablefarms_ready: setFactoryReady( object::vegetable_farm ); break;
  case all_claypit_ready: setFactoryReady( object::clay_pit ); break;
  case all_timberyard_ready: setFactoryReady( object::lumber_mill ); break;
  case all_ironmin_ready: setFactoryReady( object::iron_mine ); break;
  case all_marblequarry_ready: setFactoryReady( object::quarry ); break;
  case all_potteryworkshtp_ready: setFactoryReady( object::pottery_workshop ); break;
  case all_furnitureworksop_ready: setFactoryReady( object::furniture_workshop ); break;
  case all_weaponworkshop_ready: setFactoryReady( object::weapons_workshop ); break;
  case all_wineworkshop_ready: setFactoryReady( object::wine_workshop ); break;
  case all_oilworkshop_ready: setFactoryReady( object::oil_workshop ); break;    

  case decrease_sentiment: updateSentiment( -10 ); break;
  case increase_sentiment: updateSentiment( +10 ); break;
  case forest_grow:
  {
    auto ovs = game->city()->overlays().select<Tree>().random( 10 );

    for( auto tree : ovs )
	tree->grow();
  }
  break;

  case make_generation:
  {
    HouseList houses = game->city()->statistic().houses.find();
    for( auto house : houses )
      house->__debugMakeGeneration();
  }
  break;

  case win_mission:
  case fail_mission:
  {
    scene::Level* l = safety_cast<scene::Level*>( game->scene() );
    if( l )
    {
      Signal2<scene::Level*,bool>& foremit = (event == win_mission ? signal.winMission : signal.failedMission);
      emit foremit( l, true);
    }
  }
  break;

  case send_chastener:
  {
    world::CityPtr rome = game->empire()->capital();
    PlayerCityPtr plCity = game->city();

    world::RomeChastenerArmyPtr army = world::RomeChastenerArmy::create( game->empire() );
    army->setBase( rome );
    army->attack( ptr_cast<world::Object>( plCity ) );
  }
  break;

  case change_emperor:
  {
    GameEventPtr e = ChangeEmperor::create();
    VariantMap vm = config::load( ":/test_emperor.model" );
    e->load( vm );
    e->dispatch();
  }
  break;

  case kill_all_enemies:
  {
    auto walkers = game->city()->walkers().select<EnemySoldier>();

    for( auto wlk : walkers )
	wlk->die();
  }
  break;

  case add_city_border:   {    game->city()->tilemap().addSvkBorder();  }  break;

  case toggle_experimental_options:
  {
    bool enable = KILLSWITCH( experimental );
    SETTINGS_SET_VALUE( experimental, !enable );
  }
  break;

  case toggle_show_empireMapTiles:
  {
    bool enable = KILLSWITCH( showEmpireMapTiles );
    SETTINGS_SET_VALUE( showEmpireMapTiles, !enable );
  }
  break;

  case toggle_lock_empiremap:
  {
    bool enabled = game->empire()->isAvailable();
    game->empire()->setAvailable( !enabled );
  }
  break;

  case add_scribe_messages:
  {
    events::dispatch<ScribeMessage>( "test_message", "this is test message from yout scribes" );
  }
  break;

  case add_empire_barbarian:
  case send_barbarian_to_player:
  {
    world::BarbarianPtr brb = world::Barbarian::create( game->empire(), Point( 1000, 0 ) );

    if( event == send_barbarian_to_player )
    {
      brb->attack( game->city().as<world::Object>() );
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
    events::dispatch<RandomFire>();
  }
  break;

  case random_plague:
  {
    events::dispatch<RandomPlague>();
  }
  break;

  case random_collapse:
  {
    events::dispatch<RandomDamage>();
  }
  break;

  case earthquake:
  {
    int mapsize = game->city()->tilemap().size()-1;
    TilePos start( math::random(mapsize), math::random(mapsize) );
    TilePos stop( math::random(mapsize), math::random(mapsize) );
    events::dispatch<EarthQuake>( start, stop );
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
    events::dispatch<PostponeEvent>( "", rqvm );
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
    auto& dialog = game->gui()->add<dialog::LoadFile>( Rect(),
                                                       vfs::Path( ":/scripts/" ), ".model",
                                                       -1 );
    CONNECT( &dialog, onSelectFile(), this, Impl::runScript );
    dialog.setTitle( "Select file" );
    dialog.setText( "open" );
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
  case toggle_show_buildings : DrawOptions::instance().toggle( DrawOptions::showBuildings ); break;
  case toggle_show_trees : DrawOptions::instance().toggle( DrawOptions::showTrees ); break;
  case toggle_show_rocks : DrawOptions::instance().toggle( DrawOptions::showRocks ); break;

  case add_soldiers_in_fort:
  {
    FortList ovs = game->city()->overlays().select<Fort>();
    
    for( auto fort : ovs )
    {
      int howMuchAdd = 16 - fort->walkers().size();
      TilesArray tiles = fort->enterArea();
      for( int i=0; i < howMuchAdd; i++ )
      {
        RomeSoldierPtr soldier = Walker::create<RomeSoldier>( game->city(), walker::legionary );
        fort->addWalker( soldier.object() );
        soldier->send2city( fort, tiles.front()->pos());
      }
    }
  }
  break;
  }
}

FileChangeObserver::~FileChangeObserver()
{
  Timer::destroy( Hash(filename) );
}

void FileChangeObserver::check()
{
  if( !file.isOpen() && !filename.empty() )
  {
    file = vfs::NFile::open( filename );
    lastModify = file.lastModify();
  }

  unsigned int modifyTime = file.lastModify();
  if( lastModify != modifyTime )
  {
    lastModify = modifyTime;
    emit onChange();
  }
}

void FileChangeObserver::setFilename(const std::string& fname)
{
  Timer::destroy( Hash(filename) );

  filename = fname;
  TimerPtr timer = Timer::create( 25, true, Hash(fname) );
  CONNECT( timer, onTimeout(), this, FileChangeObserver::check )
}
