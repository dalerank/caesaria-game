function OnShowAboutEvent(title, message, time, gtype, additional) {
  game.ui.infobox.aboutEvent(title, message, time, gtype, additional);
}

game.ui.infobox.aboutEvent = function(title, message, time, gtype, additional) {
  var ibox = this.simple(0, 0, 480, 320);
  ibox.initBlackframe(18, 40, ibox.w-36, ibox.h-80);

  ibox.title = title;

  var lbText = ibox.addLabel(25, 70, ibox.blackFrame.w-50, ibox.blackFrame.h-10);
  lbText.text = message;

  var lbTime = ibox.addLabel(30, 50, ibox.blackFrame.w/2, time.format() );
  lbTime.font = "FONT_2_WHITE";

  if (additional != undefined && additional.length > 0) {
    var lbAdditional = ibox.addLabel(ibox.w/2, 50, ibox.w/2, 20);
    lbAdditional.text = additional;
    lbAdditional.textAlign = { h:"upperLeft", v:"center" };
    lbAdditional.textOffset = { x:30, y:0 };
  }

  if (gtype != undefined && gtype != g_config.good.none)
  {
    lbText.position = { x:25, y:90 };

    var ginfo = g_config.good.getInfo(gtype);
    var goodLabel = ibox.addLabel(40, 70, ibox.blackFrame.w-40, 20);
    goodLabel.text = _u(ginfo.name);
    goodLabel.textAlign = { v:"upperLeft", h:"center" };
    goodLabel.textOffset = {x:30, y:0};
    goodLabel.icon = ginfo.picture.local;
  }

  ibox.show();
  ibox.moveToCenter();
}
