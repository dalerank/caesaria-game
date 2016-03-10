sim.ui.topmenu.help.init = function() {
    var m = sim.ui.topmenu.widget.addItem("", _t("##gmenu_help##"));

    m.addItemWithCallback("##gmenu_about##", function() {
                var wnd = g_ui.addSimpleWindow(0,0,1,1);
                wnd.model = ":/gui/about.gui";
        })

    m.addItemWithCallback(_t("##gmenu_shortkeys##"), function() {
                var wnd = g_ui.addWindow(0, 0, 500, 300);
                wnd.model = ":/gui/shortkeys.gui";
                wnd.addExitButton(wnd.w-34, whd.h-34);
                wnd.moveToCenter();
                wnd.closeAfteryKey({escape:true,rmb:true});
                wnd.setModal();
        })
}

sim.ui.topmenu.options.init = function() {
    var m = sim.ui.topmenu.widget.addItem("", _t("##gmenu_options##"));

    m.addItemWithCallback("##screen_settings##", OnShowVideoSettings)
    m.addItemWithCallback("##sound_settings##",  OnShowAudioDialog)
    m.addItemWithCallback("##speed_settings##",  function() {} )
    m.addItemWithCallback("##city_settings##", function () { sim.ui.topmenu.options.showCitySettings() } )
    var constrMode = m.addItemWithCallback("##city_constr_mode##", function() {} )
    constrMode.autoChecking = true;
}

sim.ui.topmenu.file.init = function() {
    var m = sim.ui.topmenu.widget.addItem("", _t("##gmenu_file##"));

    m.addItemWithCallback("##gmenu_file_restart##",  function() { g_session.setMode(g_config.level.res_restart)})
    m.addItemWithCallback("##mainmenu_loadgame##",   OnShowSaveSelectDialog)
    m.addItemWithCallback("##gmenu_file_save##",     OnShowSaveDialog)
    m.addItemWithCallback("##gmenu_file_mainmenu##", function() { g_session.setMode(g_config.level.res_menu)})
    m.addItemWithCallback("##gmenu_exit_game##",     OnRequestExitGame)
}

sim.ui.topmenu.advisors.init = function() {

}

sim.ui.topmenu.initialize = function () {
    sim.ui.topmenu.widget = new ContextMenu("TopMenu");
    sim.ui.topmenu.file.init()
    sim.ui.topmenu.options.init()
    sim.ui.topmenu.help.init()
    sim.ui.topmenu.debug.init();
    sim.ui.topmenu.advisors.init();
}

 /* dialog::SpeedOptions& dialog = game->gui()->add<dialog::SpeedOptions>( game->timeMultiplier(),
                                                                         SETTINGS_VALUE( scrollSpeed ),
                                                                         SETTINGS_VALUE( autosaveInterval ) );

  CONNECT( &dialog, onGameSpeedChange(), game, Game::setTimeMultiplier );
  CONNECT( &dialog, onScrollSpeedChange(), renderer.camera(), Camera::setScrollSpeed );
  CONNECT( &dialog, onScrollSpeedChange(), this, Impl::saveScrollSpeed );
  CONNECT( &dialog, onAutosaveIntervalChange(), this, Impl::setAutosaveInterval ); */
