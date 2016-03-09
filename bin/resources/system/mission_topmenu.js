function createHelpMenu()
{
  var topmenu = new ContextMenu("TopMenu");

  var help = topmenu.addItem("",_t("##gmenu_help##"));
	
  help.addItemWithCallback(_t("##gmenu_about##"), function() {
    var wnd = g_ui.addSimpleWindow(0,0,1,1);
    wnd.model = ":/gui/about.gui";
    })

  help.addItemWithCallback(_t("##gmenu_shortkeys##"), function() {
        var wnd = g_ui.addWindow(0, 0, 500, 300);
    wnd.model = ":/gui/shortkeys.gui";
    wnd.addExitButton(wnd.w-34, whd.h-34);
    wnd.moveToCenter();
    wnd.closeAfteryKey({escape:true,rmb:true});
    wnd.setModal();
    })
}

function createFileMenu()
{
  var topmenu = new ContextMenu("TopMenu");

  var file = topmenu.addItem("", _t("##gmenu_file##"));

  file.addItemWithCallback(_t("##gmenu_file_restart##"),  function() { g_session.setMode(g_config.level.res_restart)})
  file.addItemWithCallback(_t("##mainmenu_loadgame##"),   OnShowSaveSelectDialog)
  file.addItemWithCallback(_t("##gmenu_file_save##"),     OnShowSaveDialog)
  file.addItemWithCallback(_t("##gmenu_file_mainmenu##"), function() { g_session.setMode(g_config.level.res_menu)})
  file.addItemWithCallback(_t("##gmenu_exit_game##"),     OnRequestExitGame)
}
