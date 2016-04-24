function OnShowTutorialWindow(path) {
  sim.ui.tutorial.show(path);
}

sim.ui.tutorial.show = function(tutorial) {
  var ibox = new InfoboxWindow(0, 0, 590, 450);
  ibox.title = _u("unknown");
  ibox.pauseGame = true;

  ibox.btnHelp = ibox.addHelpButton(12, ibox.h - 36);
  ibox.btnHelp.tooltip = _u("infobox_tooltip_help");

  var imgAdvisor = ibox.addImage(10, 10, "picture2_00001");

  var lbxHelp = ibox.addListbox(16, 76, ibox.w-32, ibox.h - 76 - 40);
  lbxHelp.itemsFont = "FONT_2_WHITE";
  lbxHelp.itemsHeight = 17;
  lbxHelp.background = true;

  var vm = {};
  if (g_config.tutorial.hasOwnProperty(tutorial)) {
    engine.log("Find tutorial for " + tutorial)
    vm = g_config.tutorial[tutorial];
  } else {
    var data = engine.load(":/tutorial/" + tutorial + ".tutorial");
    vm = JSON.parse(data);
  }

  ibox.title = vm.title;
  if (vm.sound) {
    g_session.playAudio(vm.sound, 100, g_config.audio.infobox);
  }

  if (vm.speech)  {
    ibox.addSoundMuter(5)
    ibox.addSoundEmitter(vm.speech, 100, g_config.audio.speech)
  }

  for (var i in vm.items) {
    lbxHelp.fitText(_t(vm.items[i]));
  }

  ibox.mayMove = false;
  ibox.moveToCenter();
  ibox.setModal();
}
