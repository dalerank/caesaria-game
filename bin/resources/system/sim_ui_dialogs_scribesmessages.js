sim.ui.dialogs.scribesmessages = {}

sim.ui.dialogs.scribesmessages.show = function() {
  var ibox = new InfoboxWindow(0, 0, 480, 320);
  ibox.title = _u("scribe_messages_title");

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

}
