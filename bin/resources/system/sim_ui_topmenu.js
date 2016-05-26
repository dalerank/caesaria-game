sim.ui.topmenu.labels = {}

sim.ui.topmenu.updateCityStats = function() {
  var states = g_city.states();
  sim.ui.topmenu.setPopulation(states.population)
  sim.ui.topmenu.setFunds(states.money)
  sim.ui.topmenu.setDate(g_session.date)
}

sim.ui.topmenu.help.init = function() {
    var m = sim.ui.topmenu.widget.addItem("", _ut("gmenu_help"));
    m.moveToIndex(2)

    m.addItemWithCallback(_u("gmenu_about"), function() {sim.ui.topmenu.help.aboutGame()} )
    m.addItemWithCallback(_u("gmenu_basicstrategy"), function() {sim.ui.topmenu.help.aboutStrategy()} )
    m.addItemWithCallback(_u("gmenu_shortkeys"), function() { sim.ui.topmenu.help.showHotkeys() } )
}

sim.ui.topmenu.help.aboutGame = function() {
    var wnd = g_ui.addSimpleWindow(0, 0, 500, 300)
    wnd.model = ":/gui/about.gui"
    wnd.setModal()
}

sim.ui.topmenu.help.aboutStrategy = function() {
    var wnd = g_ui.addSimpleWindow(0, 0, 500, 300)
    wnd.model = ":/gui/basicstrategy.gui"
    wnd.setModal()
}

sim.ui.topmenu.options.init = function() {
    var m = sim.ui.topmenu.widget.addItem("", _ut("gmenu_options"));
    m.moveToIndex(1)
    m.addItemWithCallback(_u("screen_settings"), function () { game.ui.dialogs.showVideoOptions() } )
    m.addItemWithCallback(_u("sound_settings"), function () { game.ui.dialogs.showAudioOptions() } )
    m.addItemWithCallback("##speed_settings##",  function() { sim.ui.dialogs.showSpeedOptions() } )
    m.addItemWithCallback("##city_settings##", function () { sim.ui.dialogs.citySettings.show() } )
    m.addItemWithCheckingCallback("##city_constr_mode##", function(checked) {
      g_city.setOption("constructorMode",checked);
      sim.ui.menu.reset();
    } )
}

sim.ui.topmenu.setPopulation = function(pop)
{
  var lbPop = sim.ui.topmenu.widget.find("#population")
  lbPop.text = _format( "{0} {1}", sim.ui.topmenu.useIcon ? "" : _ut("pop"), pop)
}

sim.ui.topmenu.setDate = function(date)
{
  var lbDate = sim.ui.topmenu.widget.find("#date")
  lbDate.text = g_session.date.format(g_session.metric)
}

sim.ui.topmenu.setFunds = function(money)
{
  var lbFunds = sim.ui.topmenu.widget.find("#funds");
  lbFunds.text = _format( "{0} {1}", sim.ui.topmenu.useIcon ? "" : _ut("denarii_short"), money)
}

sim.ui.topmenu.labels.init = function() {
  var m = sim.ui.topmenu.widget;
  var lbPopulation = new Label(m)
  var useIcon = sim.ui.topmenu.useIcom
  lbPopulation.geometry = {x:m.w-344, y:0, w:120, h:23}
  lbPopulation.name = "#population";
  lbPopulation.background = "paneling_00015";
  lbPopulation.font = "FONT_2_WHITE";
  lbPopulation.icon = { rc:useIcon ? "population" : "none", index:1 }
  lbPopulation.textAlign = { h:"center", v:"center" }
  lbPopulation.tooltip = _u("population_tooltip")

  var lbDate = new Label(m)
  lbDate.name = "#date"
  lbDate.geometry = {x:m.w-155, y:0, w:120, h:23}
  lbDate.font = "FONT_2_YELLOW"
  lbDate.textAlign = { h:"center", v:"center" }
  lbDate.background = "paneling_00015"
  lbDate.tooltip = "##date_tooltip##"

  var lbFunds = new Label(m)
  lbFunds.name = "#funds";
  lbFunds.geometry = {x:m.w-464, y:0, w:120, h:23}
  lbFunds.font = "FONT_2_WHITE"
  lbFunds.icon = { rc: useIcon ? "paneling" : "", index:332 }
  lbFunds.textAlign = { h:"center", v:"center" }
  lbFunds.background = "paneling_00015"
  lbFunds.tooltip = "##funds_tooltip##"
}

sim.ui.topmenu.help.showHotkeys = function() {
  var wnd = g_ui.addSimpleWindow(0, 0, 500, 300);
  var lbx = wnd.addKeyValueListbox(11,11,wnd.w-23,wnd.h-45)
  lbx.itemHeight = 16;

  var items = [
    "[Num +/-][increase/decrease game speed]",
    "[F5/F9][fast save/load game]",
    "[F10][make screenshot]",
    "[Ctrl+F10][fullmap screenshot]",
    "[Esc][close active window,layer or break current action]",
    "[F1/F2/F3/F4][goto position]",
    "[Ctrl+F1/F2/F3/F4][set position]",
    "[←↑↓→][move map]",
    "[Shift on building mode][build road by rectangle]",
    "[1][visit labor advisor]",
    "[2][visit military advisor]",
    "[3][visit empire advisor]",
    "[4][visit ratings advisor]",
    "[5][visit trade advisor]",
    "[6][visit population advisor]",
    "[7][visit health advisor]",
    "[8][visit education advisor]",
    "[9][visit entertainment advisor]",
    "[0][visit religion advisor]",
    "[~][visit main advisor]",
    "[key p][toggle game pause]",
    "[key f][show fire layer]",
    "[key d][show damage layer]",
    "[key c][show crime layer]",
    "[key t][show troubles layer]",
    "[key w][show water layer]",
    "[key g][show desirabilirty layer]",
    "[key h][show health layer]",
    "[key e][move camer to city enter/exit]",
    "[spacebar][show last active layer]",
    "[, (Period)][make one step (if game paused)]",
    "[. (Comma)][make 25 steps (day) (if game paused)]"
  ]

  for (var i in items)
    lbx.addLine(items[i])

  wnd.setModal();
}

sim.ui.topmenu.file.restart = function () {
  var dialog = g_ui.addConfirmationDialog( "", _u("really_restart_mission") );
  dialog.onYesCallback = function() { g_session.setMode(g_config.level.res_restart); }
}

sim.ui.topmenu.file.exitmenu = function () {
  var dialog = g_ui.addConfirmationDialog( "", _u("exit_without_saving_question") );
  dialog.onYesCallback = function() { g_session.setMode(g_config.level.res_menu); }
}

sim.ui.topmenu.file.init = function() {
    var m = sim.ui.topmenu.widget.addItem("", _t("##gmenu_file##"));
    m.moveToIndex(0)
    m.addItemWithCallback(_u("gmenu_file_restart"),  sim.ui.topmenu.file.restart);
    m.addItemWithCallback("##mainmenu_loadgame##",   lobby.ui.loadgame.loadsave );
    m.addItemWithCallback("##gmenu_file_save##",     function() { sim.ui.dialogs.savegame() } );
    m.addItemWithCallback(_u("gmenu_file_mainmenu"), sim.ui.topmenu.file.exitmenu );
    m.addItemWithCallback("##gmenu_exit_game##",     game.ui.dialogs.requestExit );
}

sim.ui.topmenu.advisors.init = function() {
    var m = sim.ui.topmenu.widget.addItem("", _t("##gmenu_advisors##"));
    m.moveToIndex(3)
    m.addItemWithCallback("##visit_labor_advisor##"      , function() { g_session.setOption("advisor",g_config.advisor.employers)} )
    m.addItemWithCallback("##visit_military_advisor##"   , function() { g_session.setOption("advisor",g_config.advisor.military)} )
    m.addItemWithCallback("##visit_imperial_advisor##"   , function() { g_session.setOption("advisor",g_config.advisor.empire)} )
    m.addItemWithCallback("##visit_rating_advisor##"     , function() { g_session.setOption("advisor",g_config.advisor.ratings)} )
    m.addItemWithCallback("##visit_trade_advisor##"      , function() { g_session.setOption("advisor",g_config.advisor.trading)} )
    m.addItemWithCallback("##visit_population_advisor##" , function() { g_session.setOption("advisor",g_config.advisor.population)} )
    m.addItemWithCallback("##visit_health_advisor##"     , function() { g_session.setOption("advisor",g_config.advisor.health)} )
    m.addItemWithCallback("##visit_education_advisor##"  , function() { g_session.setOption("advisor",g_config.advisor.education)} )
    m.addItemWithCallback("##visit_religion_advisor##"   , function() { g_session.setOption("advisor",g_config.advisor.religion)} )
    m.addItemWithCallback("##visit_entertainment_advisor##",function(){ g_session.setOption("advisor",g_config.advisor.entertainment)} )
    m.addItemWithCallback("##visit_financial_advisor##"  , function() { g_session.setOption("advisor",g_config.advisor.finance)} )
    m.addItemWithCallback("##visit_chief_advisor##"      , function() { g_session.setOption("advisor",g_config.advisor.main)} )
}

sim.ui.topmenu.initialize = function () {
    var t = sim.ui.topmenu;
    t.widget = new ContextMenu("TopMenu");
    t.widget.setDefaultStateFont(g_config.widget.state.stNormal, {size:18, color:0xff000000});
    t.widget.setDefaultStateFont(g_config.widget.state.stHovered, {size:18, color:0xffff0000});

    t.file.init()
    t.options.init()
    t.help.init()
    t.debug.init()
    t.labels.init()
    t.advisors.init()

    t.widget.setProperty("resetToDefaultFonts",1);

    game.eventmgr.bindEvent(game.events.OnDayChanged, sim.ui.topmenu.updateCityStats);
}
