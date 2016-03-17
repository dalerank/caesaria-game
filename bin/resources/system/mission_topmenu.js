function OnUpdateTopMenuCityStats() {
  var states = g_session.city.states();
  sim.ui.topmenu.setPopulation(states.population)
  sim.ui.topmenu.setFunds(states.money)
  sim.ui.topmenu.setDate(g_session.date)
}

sim.ui.topmenu.labels = {}

sim.ui.topmenu.help.init = function() {
    var m = sim.ui.topmenu.widget.addItem("", _t("##gmenu_help##"));
    m.moveToIndex(2)

    m.addItemWithCallback("##gmenu_about##", function() {
                var wnd = g_ui.addSimpleWindow(0, 0, 500, 300);
                wnd.model = ":/gui/about.gui";
                wnd.setModal();
        })

    m.addItemWithCallback(_t("##gmenu_shortkeys##"), function() { sim.ui.topmenu.help.showHotkeys() } )
}

sim.ui.topmenu.options.init = function() {
    var m = sim.ui.topmenu.widget.addItem("", _t("##gmenu_options##"));
    m.moveToIndex(1)
    m.addItemWithCallback("##screen_settings##", function () { game.ui.dialogs.showVideoOptions() } )
    m.addItemWithCallback("##sound_settings##", function () { game.ui.dialogs.showAudioOptions() } )
    m.addItemWithCallback("##speed_settings##",  function() { sim.ui.topmenu.options.showSpeedOptions() } )
    m.addItemWithCallback("##city_settings##", function () { sim.ui.topmenu.options.showCitySettings() } )
    m.addItemWithCheckingCallback("##city_constr_mode##", function(checked) { g_session.setOption("constructorMode",checked)} )
}

sim.ui.topmenu.setPopulation = function(pop)
{
  if (sim.ui.topmenu.labels.pop)
    sim.ui.topmenu.labels.pop.text = _format( "{0} {1}", sim.ui.topmenu.useIcon ? "" : _t("##pop##"), pop)
}

sim.ui.topmenu.setDate = function(date)
{
  if (sim.ui.topmenu.labels.date)
    sim.ui.topmenu.labels.date.text = g_session.date.format(g_session.metric)
}

sim.ui.topmenu.setFunds = function(money)
{
  if (sim.ui.topmenu.labels.funds)
    sim.ui.topmenu.labels.funds.text = _format( "{0} {1}", sim.ui.topmenu.useIcon ? "" : _t("##denarii_short##"), money)
}

sim.ui.topmenu.labels.init = function() {
  var m = sim.ui.topmenu.widget;
  var lbPopulation = new Label(m)
  var useIcon = sim.ui.topmenu.useIcom
  lbPopulation.geometry = {x:m.w-344, y:0, w:120, h:23}
  lbPopulation.background = "paneling_00015"
  lbPopulation.font = "FONT_2_WHITE"
  lbPopulation.icon = { rc:useIcon ? "population" : "none", index:1 }
  lbPopulation.textAlign = { h:"center", v:"center" }
  lbPopulation.tooltip = "##population_tooltip##"

  sim.ui.topmenu.labels.pop = lbPopulation

  var lbDate = new Label(m)
  lbDate.geometry = {x:m.w-155, y:0, w:120, h:23}
  lbDate.font = "FONT_2_YELLOW"
  lbDate.textAlign = { h:"center", v:"center" }
  lbDate.background = "paneling_00015"
  lbDate.tooltip = "##date_tooltip##"

  sim.ui.topmenu.labels.date = lbDate

  var lbFunds = new Label(m)
  lbFunds.geometry = {x:m.w-464, y:0, w:120, h:23}
  lbFunds.font = "FONT_2_WHITE"
  lbFunds.name = "lbFunds"
  lbFunds.icon = { rc: useIcon ? "paneling" : "", index:332 }
  lbFunds.textAlign = { h:"center", v:"center" }
  lbFunds.background = "paneling_00015"
  lbFunds.tooltip = "##funds_tooltip##"

  sim.ui.topmenu.labels.funds = lbFunds
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

sim.ui.topmenu.file.init = function() {
    var m = sim.ui.topmenu.widget.addItem("", _t("##gmenu_file##"));
    m.moveToIndex(0)
    m.addItemWithCallback("##gmenu_file_restart##",  function() { g_session.setMode(g_config.level.res_restart)})
    m.addItemWithCallback("##mainmenu_loadgame##",   function() { lobby.ui.loadgame.loadsave()})
    m.addItemWithCallback("##gmenu_file_save##",     function() { game.ui.dialogs.savegame()})
    m.addItemWithCallback("##gmenu_file_mainmenu##", function() { g_session.setMode(g_config.level.res_menu)})
    m.addItemWithCallback("##gmenu_exit_game##",     function() { game.ui.dialogs.requestExit()})
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
    t.file.init()
    t.options.init()
    t.help.init()
    t.debug.init()
    t.labels.init()
    t.advisors.init()

    OnUpdateTopMenuCityStats()
}
