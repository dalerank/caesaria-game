sim.ui.dialogs.scribesmessages = {}

sim.ui.dialogs.scribesmessages.show = function() {
  var window = g_ui.find("ScribesMessages");

  if (window != null) {
    window.bringToFront();
    return;
  }

  engine.log("sim.ui.dialogs.scribesmessages.show");
  var ibox = new Window(0);
  ibox.geometry = { x:0, y:0, w:480, h:320 };
  ibox.addExitButton(ibox.w-36, ibox.h-36);
  ibox.moveToCenter();
  ibox.closeAfterKey({escape:true});

  ibox.title = _u("scribe_messages_title");
  ibox.name = "ScribesMessages";

  ibox.btnHelp = ibox.addHelpButton(12, ibox.h - 36);
  ibox.btnHelp.tooltip = _u("infobox_tooltip_help");

  var lbDate = ibox.addLabel(80, 43, ibox.w/2, 15);
  lbDate.text = _u("scrw_date");
  lbDate.font = "FONT_1";

  var lbSubj = ibox.addLabel(ibox.w/2, 43, ibox.w/2-30, 15);
  lbSubj.text = _u("scrw_subject");
  lbSubj.font = "FONT_1";

  var lbInfo = ibox.addLabel(50, ibox.h-50, ibox.w-100, 40);
  lbInfo.textAlign = {v:"upperLeft", h:"upperLeft"};
  lbInfo.font = "FONT_1";
  lbInfo.text = _u("left_click_open_right_erase");
  lbInfo.multiline = true;

  ibox.lbxMessages = ibox.addListbox( 16, 60, ibox.w - 32, ibox.h - 110);
  ibox.lbxMessages.background = true;

  ibox.lbxMessages.onRmbSelectedCallback = function(index) {
    var scribes = g_city.scribes();
    scribes.removeMessage(index);
    ibox.update();
  }

  ibox.lbxMessages.onSelectedCallback = function(index) {
    var scribes = g_city.scribes();
    var message = scribes.getMessage(index);
    scribes.readMessage(index);
    game.ui.infobox.aboutEvent(message.title, message.text, message.date, message.good, message.adv);
    ibox.update();
  }

  g_session.playAudio("extm_scribes_00001", 100, g_config.audio.effects )

  ibox.update = function() {
    ibox.lbxMessages.clear();

    var scribes = g_city.scribes();

    if (scribes.getMessagesNumber() == 0) {
      lbDate.display = false;
      lbSubj.display = false;
      lbInfo.text = _u("srcw_no_messages");
      return;
    }

    ibox.lbxMessages.itemsFont = "FONT_1";
    ibox.lbxMessages.setItemDefaultColor("hovered", "0xffff0000");
    for (var i=0; i < scribes.getMessagesNumber(); i++) {
      var message = scribes.getMessage(i);
      var text = _format("{0}  {1}", message.date.format(g_config.metric.mode), message.title);
      var imgIndex = (message.critical ? 113 : 111) + (message.opened ? 1 : 0);

      var index = ibox.lbxMessages.addLine(text);
      ibox.lbxMessages.setItemOverrideColor(index, g_config.colors.red, ibox.lbxMessages.flag.itemHovered )
      engine.log(index);

      var picture = g_render.picture("paneling", imgIndex).fallback("paneling", 1);
      engine.log(picture.name());
      ibox.lbxMessages.setItemIcon(index, picture, {x:2, y:2});
    }
  }

  ibox.update();
  ibox.setModal();
  ibox.mayMove = false;
}
