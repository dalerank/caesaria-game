sim.ui.tutorial.show = function() {
  var ibox = new InfoboxWindow(0, 0, 590, 450)
  ibox.title = _u("unknown");
  ibox.pauseGame = true;

  ibox.btnHelp = ibox.addHelpButton(12, ibox.h - 36);
  ibox.btnHelp.tooltip = _u("infobox_tooltip_help");

  ibox.mayMove = false;
  ibox.moveToCenter();
  ibox.setModal();

  var imgAdvisor = ibox.addImage(10, 10, "picture2_00001");

  var lbxHelp = ibox.addListBox(16, 76, ibox.w-32, 410);
  lbxHelp.itemsFont = "FONT_2_WHITE";
  lbxHelp.itemsHeight = 17;
}
