sim.ui.dialogs.scribesmessages = {}

sim.ui.dialogs.scribesmessages.show = function() {
  var window = g_ui.find("ScribesMessages");
  if (window != null) {
    window.bringToFront();
    return;
  }

  var ibox = new InfoboxWindow(0, 0, 480, 320);
  ibox.title = _u("scribe_messages_title");
  ibox.name = "ScribesMessages";

  ibox.btnHelp = ibox.addHelpButton(12, ibox.h - 36);
  ibox.btnHelp.tooltip = _u("infobox_tooltip_help");

  var lbDate = ibox.addLabel(80, 45, ibox.w/2, 60);
  lbDate.text = _u("scrw_date");
  lbDate.font = "FONT_1";

  var lbSubj = ibox.addLabel( ibox.w/2, 45, ibox.w/2-30, 60);
  lbSubj.text = _u("scrw_subject");
  lbSubj.font = "FONT_1";

  var lbInfo = ibox.addLabel(50, ibox.h-40, ibox.w-100, 28);
  lbInfo.textAlign = {v:"center", h:"upperLeft"};
  lbInfo.font = "FONT_1";
  lbInfo.text = _u("left_click_open_right_erase");
  lbInfo.multiline = true;

  ibox.lbxMessages = ibox.addListbox( 16, 60, ibox.w - 16, ibox.h - 50);
  ibox.background = true;

  ibox.lbxMessages.onRmbSelectedCallback = function(index) {
    var scribes = g_sesssion.city.scribes();
    scribes.removeMessage(index);
    ibox.update();
  }

  ibox.lbxMessages.onSelectedCallback = function(index) {
    var scribes = g_sesssion.city.scribes();
    var message = scribes.getMessage(index);
    scribes.readMessage(index);
    game.ui.infobox.aboutEvent(message.title, message.text, message.date, message.good, message.adv);
    ibox.update();
  }

  g_session.playAudio("extm_scribes_00001", 100, g_config.audio.effects )

  ibox.update = function() {
    ibox.lbxMessages.clear();

    var scribes = g_sesssion.city.scribes();
    var message = scribes.getMessage(0);

    if (message == undefined) {
      lbDate.display = false;
      lbSubj.display = false;
      lbInfo.text = _u("srcw_no_messages");
      return;
    }

    lbxMessages.itemsFont = "FONT_1";
    lbxMessages.setItemDefaultColor("hovered", "0xffff0000");
    for (var i=0; i < 100; i++)
    {
      message = scribes.getMessage(i);
      var text = _format("{0}  {1}", message.date.format(g_config.metric.mode), mt.title);
      var imgIndex = (message.critical ? 113 : 111) + (opened ? 1 : 0);

      var index = lbxMessages.addLine(text);
      var picture = g_render.picture("paneling", imgIndex).fallback("paneling", 1);
      lbxMessages.setItemIcon(index, picture, {x:2, y:2});
    }
  }

  ibox.update();
}
