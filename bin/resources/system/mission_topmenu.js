sim.ui.topmenu.help.init = function() {
    var m = sim.ui.topmenu.widget.addItem("", _t("##gmenu_help##"));
    m.moveToIndex(2)

    m.addItemWithCallback("##gmenu_about##", function() {
                var wnd = g_ui.addSimpleWindow(0, 0, 500, 300);
                wnd.model = ":/gui/about.gui";
                wnd.setModal();
        })

    m.addItemWithCallback(_t("##gmenu_shortkeys##"), function() {
                var wnd = g_ui.addSimplWindow(0, 0, 500, 300);
                wnd.model = ":/gui/shortkeys.gui";
                wnd.setModal();
        })
}

sim.ui.topmenu.options.init = function() {
    var m = sim.ui.topmenu.widget.addItem("", _t("##gmenu_options##"));
    m.moveToIndex(1)
    m.addItemWithCallback("##screen_settings##", function () { game.ui.dialogs.showVideoOptions() } )
    m.addItemWithCallback("##sound_settings##", function () { game.ui.dialogs.showAudioOptions() } )
    m.addItemWithCallback("##speed_settings##",  function() { sim.ui.topmenu.options.showSpeedOptions() } )
    m.addItemWithCallback("##city_settings##", function () { sim.ui.topmenu.options.showCitySettings() } )
    var constrMode = m.addItemWithCallback("##city_constr_mode##", function() {} )
    constrMode.autoChecking = true;
}

sim.ui.topmenu.file.init = function() {
    var m = sim.ui.topmenu.widget.addItem("", _t("##gmenu_file##"));
    m.moveToIndex(0)
    m.addItemWithCallback("##gmenu_file_restart##",  function() { g_session.setMode(g_config.level.res_restart)})
    m.addItemWithCallback("##mainmenu_loadgame##",   function() { lobby.ui.loadgame.loadsave()})
    m.addItemWithCallback("##gmenu_file_save##",     function() { game.ui.dialogs.savegame()})
    m.addItemWithCallback("##gmenu_file_mainmenu##", function() { g_session.setMode(g_config.level.res_menu)})
    m.addItemWithCallback("##gmenu_exit_game##",     OnRequestExitGame)
}

sim.ui.topmenu.advisors.init = function() {

}

sim.ui.topmenu.initialize = function () {
    var t = sim.ui.topmenu;
    t.widget = new ContextMenu("TopMenu");
    t.file.init()
    t.options.init()
    t.help.init()
    t.debug.init();
    t.advisors.init();
}
