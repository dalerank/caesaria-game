game.ui.dialogs.festivalplaning = {}

game.ui.dialogs.festivalplaning.gods = [
  { type : g_config.gods.ceres, rc: 17},
  { type : g_config.gods.neptune, rc: 18},
  { type : g_config.gods.mercury, rc: 19},
  { type : g_config.gods.mars, rc: 20},
  { type : g_config.gods.venus, rc: 21},
]

game.ui.dialogs.festivalplaning.changeType = function(type) {
  var ibox = game.ui.dialogs.festivalplaning.widget;
  ibox.btnSmallFestival.pressed = (type.id == g_config.festival.small.id);
  ibox.btnMiddleFestival.pressed= (type.id == g_config.festival.middle.id);
  ibox.btnGreatFestival.pressed = (type.id == g_config.festival.big.id);

  ibox.festivaleType = type.id;
  ibox.lbFestivalName.text = _ut(type.title);
}

game.ui.dialogs.festivalplaning.show = function() {
  var ibox = g_ui.addSimpleWindow(0, 0, 625, 320);
  ibox.moveToCenter();
  ibox.setModal();
  ibox.mayMove = false;
  ibox.divinity = g_config.gods.ceres;

  ibox.title = _u("title");

  ibox.lbFestivalName = ibox.addLabel(165, ibox.h-48, 200, 20);
  ibox.lbFestivalName.text = _u("small_festival")
  ibox.lbFestivalName.font = "FONT_2";
  ibox.lbFestivalName.textAlign =  { v:"center", h:"center" };

  ibox.btnSmallFestival = ibox.addButton(95, 170, ibox.w-190, 25);
  ibox.btnSmallFestival.bistate = true;
  ibox.btnSmallFestival.style = "whiteBorderUp";
  ibox.btnSmallFestival.text = _format( "{0} {1}", _ut("small_festival"),
                                        game.religion.getFestivalCost(g_config.festival.small));
  ibox.btnSmallFestival.textAlign = { v:"center", h:"center" };
  ibox.btnSmallFestival.callback = function() { game.ui.dialogs.festivalplaning.changeType(g_config.festival.small)}

  ibox.btnMiddleFestival = ibox.addButton(95, 200, ibox.w-190, 25);
  ibox.btnMiddleFestival.bistate = true;
  ibox.btnMiddleFestival.style = "whiteBorderUp";
  ibox.btnMiddleFestival.text = _format( "{0} {1}", _ut("middle_festival"),
                                          game.religion.getFestivalCost(g_config.festival.middle));
  ibox.btnMiddleFestival.textAlign = { v:"center", h:"center" }
  ibox.btnMiddleFestival.callback = function() { game.ui.dialogs.festivalplaning.changeType(g_config.festival.middle)}

  ibox.btnGreatFestival = ibox.addButton(95, 230, ibox.w-190, 25);
  ibox.btnGreatFestival.bistate = true;
  ibox.btnGreatFestival.style = "whiteBorderUp";
  ibox.btnGreatFestival.text = _format( "{0} {1}", _ut("great_festival"),
                                        game.religion.getFestivalCost(g_config.festival.big));
  ibox.btnGreatFestival.textAlign = { v:"center", h:"center" };
  ibox.btnGreatFestival.callback = function() { game.ui.dialogs.festivalplaning.changeType(g_config.festival.big)}

  ibox.updateTitle = function() {
    var divinity = game.gods.roman[ibox.divinity];
    ibox.title = _format( "##hold_{0}_festival##", divinity.internalName());
  }

  var btnHelp = ibox.addHelpButton(12, ibox.h-36);
  btnHelp.uri = "festival_adv";

  var btnYes = ibox.addTexturedButton(350, ibox.h - 50, 39, 26);
  btnYes.tooltip = _u("new_festival")
  btnYes.states = { rc:"paneling", normal:239, hover:240, pressed:241, disabled:242 };
  btnYes.callback = function() {
    game.religion.assignFestival(ibox.divinity, ibox.festivaleType);
    ibox.deleteLater();

    var widget = new Window("EntertainmentAdvisor");
    if (widget) {
      widget.setWindowFlag("updateFestivalInfo", true);
    }
  }

  var btnNo = ibox.addTexturedButton(350+43, ibox.h - 50, 39, 26);
  btnNo.tooltip = _u("donot_organize_festival");
  btnNo.states = { rc:"paneling", normal:243, hover:244, pressed:245, disabled:246 };
  btnNo.callback = function() { ibox.deleteLater(); }

  ibox.godBtns = [];
  ibox.addGodImage = function(who, column, imageIndex) {
    var btn = ibox.addTexturedButton(column * 100 + 60, 48, 81, 91);
    btn.states = {rc:"panelwindows", normal:imageIndex, hover:imageIndex, pressed:imageIndex+5, disabled:imageIndex+5};
    btn.bistate = true;
    btn.tooltip = _u("arrange_festiable_for_this_god");
    ibox.godBtns.push(btn);
    btn.callback = function() {
      ibox.divinity = who;
      for (var i in ibox.godBtns) {
        ibox.godBtns[i].pressed = false;
      }
      btn.pressed = true;
      ibox.updateTitle();
    }

    if (who == g_config.gods.ceres)
      btn.pressed = true;
  }

  for (var i in game.ui.dialogs.festivalplaning.gods) {
    var config = game.ui.dialogs.festivalplaning.gods[i];
    ibox.addGodImage(config.type, i, config.rc);
  }

  ibox.updateTitle();
  game.ui.dialogs.festivalplaning.widget = ibox;
}
