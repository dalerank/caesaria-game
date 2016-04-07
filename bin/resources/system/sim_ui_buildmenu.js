function OnShowBuildMenu(type, parentName) {
  sim.ui.buildmenu.show(type);
}

sim.ui.buildmenu.show = function(type, parentName) {
  var menus = g_ui.find("BuildMenu");
  if (menus != null)
    menus.deleteLater();

  var extMenu = g_ui.find("ExtentMenu");
  var parent = g_ui.find(parentName);
  var buildMenu = new Widget("Widget");
  buildMenu.internalName = "BuildMenu";

  if (buildMenu != null) {
    buildMenu.setNotClipped(true);
    buildMenu.init(type);
    buildMenu.internalName = "BuildMenu";
    var resolution = g_session.resolution;
    buildMenu.geometry = { x:0, y:0, w:resolution.w-extMenu.w, h:resolution.h ) );
  }
}
