sim.ui.topmenu.debug.reset = function()
{
  sim.ui.topmenu.debugmenu.clear()
  sim.ui.topmenu.init()
}

sim.ui.topmenu.debug.init = function()
{
  var topmenu = sim.ui.topmenu.widget;
  var d = sim.ui.topmenu.debug;
  sim.ui.topmenu.debugmenu = topmenu.findItem("Debug");

  /*topmenu.addItemWithCallback("Debug/enemies", "add_enemy_archers", function() {engine.log("test")} )
  topmenu.addItemWithCallback("Debug/enemies", "add_enemy_soldiers", function() {engine.log("test")} )
  topmenu.addItemWithCallback("Debug/enemies", "add_chastener_soldiers", function() {engine.log("test")} )
  topmenu.addItemWithCallback("Debug/enemies", "add_wolves", function() {engine.log("test")} )
  topmenu.addItemWithCallback("Debug/enemies", "send_chastener", function() {engine.log("test")} )
  topmenu.addItemWithCallback("Debug/enemies", "add_empire_barbarian", function() {engine.log("test")} )
  topmenu.addItemWithCallback("Debug/enemies", "send_barbarian_to_player", function() {engine.log("test")} )
  topmenu.addItemWithCallback("Debug/enemies", "kill_all_enemies", function() {engine.log("test")} )

  topmenu.addItemWithCallback("Debug/requests", "comply_rome_request", function() {engine.log("test")} )
  topmenu.addItemWithCallback("Debug/requests", "test_request", function() {engine.log("test")} )

  topmenu.addItemWithCallback("Debug/divinity", "send_mars_wrath", function() {engine.log("test")} )
  topmenu.addItemWithCallback("Debug/divinity", "send_mars_spirit", function() {engine.log("test")} )
  topmenu.addItemWithCallback("Debug/divinity", "send_venus_wrath", function() {engine.log("test")} )
  topmenu.addItemWithCallback("Debug/divinity", "send_neptune_wrath", function() {engine.log("test")} )
  topmenu.addItemWithCallback("Debug/divinity", "send_venus_smallcurse", function() {engine.log("test")} )*/

  topmenu.addItemWithCallback("Debug/money", "Add 1000 dn to city", function() {
      engine.log("Added 1000 dn to city")
      g_session.createIssue("donation",1000)
  } )
  topmenu.addItemWithCallback("Debug/money", "Add 1000 dn to player", function() {
    g_session.player.appenMoney(1000);
  } )

  /*topmenu.addItemWithCallback("Debug/goods", "add_wheat_to_warehouse", function() {engine.log("test")} )
  topmenu.addItemWithCallback("Debug/goods", "add_fish_to_warehouse", function() {engine.log("test")} )
  topmenu.addItemWithCallback("Debug/goods", "add_meat_to_warehouse", function() {engine.log("test")} )
  topmenu.addItemWithCallback("Debug/goods", "add_olives_to_warehouse", function() {engine.log("test")} )
  topmenu.addItemWithCallback("Debug/goods", "add_fruit_to_warehouse", function() {engine.log("test")} )
  topmenu.addItemWithCallback("Debug/goods", "add_grape_to_warehouse", function() {engine.log("test")} )
  topmenu.addItemWithCallback("Debug/goods", "add_vegetable_to_warehouse", function() {engine.log("test")} )
  topmenu.addItemWithCallback("Debug/goods", "add_clay_to_warehouse", function() {engine.log("test")} )
  topmenu.addItemWithCallback("Debug/goods", "add_timber_to_warehouse", function() {engine.log("test")} )
  topmenu.addItemWithCallback("Debug/goods", "add_iron_to_warehouse", function() {engine.log("test")} )
  topmenu.addItemWithCallback("Debug/goods", "add_marble_to_warehouse", function() {engine.log("test")} )
  topmenu.addItemWithCallback("Debug/goods", "add_pottery_to_warehouse", function() {engine.log("test")} )
  topmenu.addItemWithCallback("Debug/goods", "add_furniture_to_warehouse", function() {engine.log("test")} )
  topmenu.addItemWithCallback("Debug/goods", "add_weapons_to_warehouse", function() {engine.log("test")} )
  topmenu.addItemWithCallback("Debug/goods", "add_wine_to_warehouse", function() {engine.log("test")} )
  topmenu.addItemWithCallback("Debug/goods", "add_oil_to_warehouse", function() {engine.log("test")} )

  topmenu.addItemWithCallback("Debug/goods", "add_wheat_to_granary", function() {engine.log("test")} )
  topmenu.addItemWithCallback("Debug/goods", "add_fish_to_granary", function() {engine.log("test")} )
  topmenu.addItemWithCallback("Debug/goods", "add_meat_to_granary", function() {engine.log("test")} )
  topmenu.addItemWithCallback("Debug/goods", "add_fruit_to_granary", function() {engine.log("test")} )
  topmenu.addItemWithCallback("Debug/goods", "add_vegetable_to_granary", function() {engine.log("test")} )

  topmenu.addItemWithCallback("Debug/factories", "all_wheatfarms_ready", function() {engine.log("test")} )
  topmenu.addItemWithCallback("Debug/factories", "all_wahrf_ready", function() {engine.log("test")} )
  topmenu.addItemWithCallback("Debug/factories", "all_olivefarms_ready", function() {engine.log("test")} )
  topmenu.addItemWithCallback("Debug/factories", "all_fruitfarms_ready", function() {engine.log("test")} )
  topmenu.addItemWithCallback("Debug/factories", "all_grapefarms_ready", function() {engine.log("test")} )
  topmenu.addItemWithCallback("Debug/factories", "all_vegetablefarms_ready", function() {engine.log("test")} )
  topmenu.addItemWithCallback("Debug/factories", "all_claypit_ready", function() {engine.log("test")} )
  topmenu.addItemWithCallback("Debug/factories", "all_timberyard_ready", function() {engine.log("test")} )
  topmenu.addItemWithCallback("Debug/factories", "all_ironmine_ready", function() {engine.log("test")} )
  topmenu.addItemWithCallback("Debug/factories", "all_marblequarry_ready", function() {engine.log("test")} )
  topmenu.addItemWithCallback("Debug/factories", "all_potteryworkshtp_ready", function() {engine.log("test")} )
  topmenu.addItemWithCallback("Debug/factories", "all_furnitureworksop_fillstock", function() {engine.log("test")} )
  topmenu.addItemWithCallback("Debug/factories", "all_furnitureworksop_ready", function() {engine.log("test")} )
  topmenu.addItemWithCallback("Debug/factories", "all_weaponworkshop_ready", function() {engine.log("test")} )
  topmenu.addItemWithCallback("Debug/factories", "all_wineworkshop_ready", function() {engine.log("test")} )
  topmenu.addItemWithCallback("Debug/factories", "all_oilworkshop_ready", function() {engine.log("test")} )
  topmenu.addItemWithCallback("Debug/factories", "all_creamery_fillstock", function() {engine.log("test")} )

  topmenu.addItemWithCallback("Debug/other", "send_player_army", function() {engine.log("test")} )
  topmenu.addItemWithCallback("Debug/other", "screenshot", function() {engine.log("test")} )
  topmenu.addItemWithCallback("Debug/other", "enable_constructor_mode", function() {engine.log("test")} )
  topmenu.addItemWithCallback("Debug/other", "next_theme", function() {engine.log("test")} )*/

  topmenu.addItemWithCallback("Debug/buildings", "toggle_shipyard_enable", function() { d.toggleBuildOptions("shipyard")} )
  topmenu.addItemWithCallback("Debug/buildings", "toggle_reservoir_enable", function() { d.toggleBuildOptions("reservoir")} )
  topmenu.addItemWithCallback("Debug/buildings", "toggle_wineshop_enable", function() { d.toggleBuildOptions("wine_workshop")} )
  topmenu.addItemWithCallback("Debug/buildings", "toggle_vinard_enable", function() { d.toggleBuildOptions("vinard")} )

  /*topmenu.addItemWithCallback("Debug/disaster", "random_fire", function() {engine.log("test")} )
  topmenu.addItemWithCallback("Debug/disaster", "random_collapse", function() {engine.log("test")} )
  topmenu.addItemWithCallback("Debug/disaster", "random_plague", function() {engine.log("test")} )
  topmenu.addItemWithCallback("Debug/disaster", "earthquake", function() {engine.log("test")} )
  topmenu.addItemWithCallback("Debug/disaster", "fill_random_claypit", function() {engine.log("test")} )
  topmenu.addItemWithCallback("Debug/disaster", "forest_fire", function() {engine.log("test")} )*/

  topmenu.addItemWithCallback("Debug/level", "win_mission", function() { sim.ui.missionWin() } )
  topmenu.addItemWithCallback("Debug/level", "fail_mission", function() { sim.ui.missionLose() } )

  /*topmenu.addItemWithCallback("Debug/level", "change_emperor", function() {engine.log("test")} )
  topmenu.addItemWithCallback("Debug/level", "property_browser", function() {engine.log("test")} )
  topmenu.addItemWithCallback("Debug/level", "show_requests", function() {engine.log("test")} )
  topmenu.addItemWithCallback("Debug/level", "show_attacks", function() {engine.log("test")} )

  topmenu.addItemWithCallback("Debug/empire", "send_merchants", function() {engine.log("test")} )
  topmenu.addItemWithCallback("Debug/empire", "toggle_lock_empiremap", function() {engine.log("test")} )
  topmenu.addItemWithCallback("Debug/empire", "empire_toggle_capua", function() {engine.log("test")} )
  topmenu.addItemWithCallback("Debug/empire", "empire_toggle_londinium", function() {engine.log("test")} )*/

  topmenu.addItemWithCallback("Debug/city", "Toggle gods", function() { d.toggleCityFlag("godEnabled")} )
  topmenu.addItemWithCallback("Debug/city", "Toggle adviser", function() { d.toggleCityFlag("adviserEnabled")} )
  /*topmenu.addItemWithCallback("Debug/city", "add_soldiers_in_fort", function() {engine.log("test")} )
  topmenu.addItemWithCallback("Debug/city", "add_city_border", function() {engine.log("test")} )
  topmenu.addItemWithCallback("Debug/city", "crash_favor", function() {engine.log("test")} )
  topmenu.addItemWithCallback("Debug/city", "add_scribe_messages", function() {engine.log("test")} )
  topmenu.addItemWithCallback("Debug/city", "show_fest", function() {engine.log("test")} )*/
  topmenu.addItemWithCallback("Debug/city", "add_favor", function() { g_session.emperor.updateRelation( g_session.city.name(), 10)} )
  topmenu.addItemWithCallback("Debug/city", "remove_favor", function() { g_session.emperor.updateRelation( g_session.city.name(), -10)} )

  topmenu.addItemWithCallback("Debug/windows", "emperor gift", function() {game.ui.dialogs.gift2emperor.show()} )
 /*topmenu.addItemWithCallback("Debug/city", "make_generation", function() {engine.log("test")} )
  topmenu.addItemWithCallback("Debug/city", "decrease_sentiment", function() {engine.log("test")} )
  topmenu.addItemWithCallback("Debug/city", "increase_sentiment", function() {engine.log("test")} )
  topmenu.addItemWithCallback("Debug/city", "forest_grow", function() {engine.log("test")} )
  topmenu.addItemWithCallback("Debug/city", "reset_fire_risk", function() {engine.log("test")} )
  topmenu.addItemWithCallback("Debug/city", "reset_collapse_risk", function() {engine.log("test")} )

  topmenu.addItemWithCallback("Debug/house", "increase_max_level", function() {engine.log("test")} )
  topmenu.addItemWithCallback("Debug/house", "decrease_max_level", function() {engine.log("test")} )
  topmenu.addItemWithCallback("Debug/house", "increase_house_level", function() {engine.log("test")} )
  topmenu.addItemWithCallback("Debug/house", "decrease_house_level", function() {engine.log("test")} )
  topmenu.addItemWithCallback("Debug/house", "lock_house_level", function() {engine.log("test")} )*/

  topmenu.addItemWithCallback("Debug/options", "all_sound_off", function() {
      engine.setVolume(1,0); engine.setOption("ambientVolume",0);
      engine.setVolume(2,0); engine.setOption("musicVolume",0);
      engine.setVolume(3,0); engine.setOption("soundVolume",0);
  } )

  /*topmenu.addItemWithCallback("Debug/options", "reload_aqueducts", function() {engine.log("test")} )
  topmenu.addItemWithCallback("Debug/options", "toggle_experimental_options", function() {engine.log("test")} )
  topmenu.addItemWithCallback("Debug/options", "reload_buildings_config", function() {engine.log("test")} )*/

  topmenu.addItemWithCallback("Debug/draw", "toggle_grid_visibility", function() { d.toggleDrawFlag("drawGrid");} )
  topmenu.addItemWithCallback("Debug/draw", "toggle_overlay_base", function() { d.toggleDrawFlag("shadowOverlay")} )
  topmenu.addItemWithCallback("Debug/draw", "toggle_show_path", function() { d.toggleDrawFlag("showPath")} )
  topmenu.addItemWithCallback("Debug/draw", "toggle_show_roads", function() { d.toggleDrawFlag("showRoads")} )
  topmenu.addItemWithCallback("Debug/draw", "toggle_show_buildings", function() { d.toggleDrawFlag("showBuildings")} )
  topmenu.addItemWithCallback("Debug/draw", "toggle_show_trees", function() { d.toggleDrawFlag("showTrees")} )
  topmenu.addItemWithCallback("Debug/draw", "toggle_show_object_area", function() { d.toggleDrawFlag("showObjectArea")} )
  topmenu.addItemWithCallback("Debug/draw", "toggle_show_walkable_tiles", function() { d.toggleDrawFlag("showWalkableTiles")} )
  topmenu.addItemWithCallback("Debug/draw", "toggle_show_locked_tiles", function() { d.toggleDrawFlag("showLockedTiles")} )
  topmenu.addItemWithCallback("Debug/draw", "toggle_show_flat_tiles", function() { d.toggleDrawFlag("showFlatTiles")} )
  topmenu.addItemWithCallback("Debug/draw", "toggle_show_rocks", function() { d.toggleDrawFlag("showRocks")} )

  //topmenu.addItemWithCallback("Debug/empiremap", "toggle_show_empireMapTiles", function() {engine.log("test")} )

  //topmenu.addItemWithCallback("Debug/steam", "reset_steam_prefs", function() {engine.log("test")} )
}

sim.ui.topmenu.debug.toggleBuildOptions = function(name) {
  var value = g_session.city.getBuildOption(name);
  g_session.city.setBuildOption(name,!value);

  engine.log( "Change build flag " + name + " to " + (!value ? "true" : "false"))
}

sim.ui.topmenu.debug.toggleCityFlag = function(name) {
  var value = g_session.city.getOption(name)
  g_session.city.setOption(name, !value)

  engine.log( "Change city flag " + name + " to " + (!value ? "true" : "false"))
}

sim.ui.topmenu.debug.toggleDrawflag = function(name) {
  var value = g_session.getAdvflag(name);
  g_session.setAdvflag(name, !value);

  engine.log( "Change draw flag " + name + " to " + (!value ? "true" : "false"))
}

//was call on reload script
if (sim.ui.topmenu.debugmenu)
  sim.ui.topmenu.debug.reset()
