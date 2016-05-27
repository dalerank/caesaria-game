sim.ui.advisors.emperor = {}

sim.ui.advisors.emperor.getEmperorFavourStr = function() {
  var favourLimiter=20;
  var maxFavourValue=100;
  var index = g_city.favor * favourLimiter / maxFavourValue;
  return _format("##emperor_favour_{0}{1}##", Math.floor(index/10), Math.floor(index%10));
}

sim.ui.advisors.emperor.updatePrimaryFunds = function()
{
  var widget = g_ui.find("#primaryFunds");
  if (widget != null)
    widget = _format( "{0} {1} {2}", _ut("primary_funds"),
                                     g_player.mayor.money(),
                                     _ut("denarii_short"));
}

sim.ui.advisors.emperor.show = function() {
  var parlor = g_ui.find("ParlorWindow");

  sim.ui.advisors.hide();

  var resolution = g_session.resolution;
  var w = new Window(parlor);
  w.name = "#advisorWindow";

  w.geometry = {x:0, y:0, w:640, h:432};
  w.title = _u("emperor_advisor_title");
  w.x = (resolution.w - w.w)/2;
  w.y = resolution.h/2 - 242;
  w.mayMove = false;

  var lbIcon = w.addLabel(10, 10, 50, 50);
  lbIcon.icon = {rc:"paneling", index:257};

  var lbBlackframe = w.addLabel(35, 80, w.w-70, 70);
  lbBlackframe.style = "blackFrame";

  w.btnHelp = w.addHelpButton(12, w.h - 36);
  w.btnHelp.uri = "emperor_advisor";

  var bgRequests = w.addLabel(32, 91, 602, 310);
  bgRequests.style = "blackFrame";

  var btnRqHistory = w.addhButton(bgRequests.w-84, bgRequests.h-28, 80, 24);
  btnRqHistory.text = "##rqs_history##";
  btnRqHistory.style = "blackBorderUp";

  var bgActions = w.addLabel(66, 325, 510, 100)
  bgActions.style = "blackFrame"

  var lbEmperorFavour = w.addLabel(58, 44, 550, 20);
  lbEmperorFavour.text = _format( "{0} {1}", _ut("advemp_emperor_favour"), g_city.favor )
  lbEmperorFavour.font = "FONT_2"

  var lbEmperorFavourDesc = w.addLabel(58, 64, 550, 20)
  lbEmperorFavourDesc.font = "FONT_2"
  lbEmperorFavourDesc.text = this.getEmperorFavourStr();

  var lbPost = w.addLabel(70, 336, 220, 26)
  lbPost.text = "Post"
  lbPost.font = "FONT_2"

  var btnGiftHistory = w.addButton(290, 343, 90, 20)
  btnGiftHistory.text = "##history_gift##"
  btnGiftHistory.style = "blackBorderUp"

  var btnSendGift = w.addButton(382, 343, 90, 20)
  btnSendGift.text = "##dispatch_gift##"
  btnSendGift.style = "blackBorderUp"
  btnSendGift.callback = function() { OnShowEmperorGiftWindow(); }

  var lbPrimaryFunds = w.addLabel(70, 370, 240, 20)
  lbPrimaryFunds.text = "##primary_funds##"
  lbPrimaryFunds.font = "FONT_2"
  lbPrimaryFunds.name = "#primaryFunds"

  var btnSend2City = w.addButton(322, 370, 250, 20)
  btnSend2City.text = "##send_to_city##"
  btnSend2City.style = "blackBorderUp"
  btnSend2City.callback = function() { ShowPlayer2CityDonationWindow(); }

  var btnChangeSalary = w.addButton(70, 395, 500, 20)
  btnChangeSalary.text = "##set_mayor_salary##"
  btnChangeSalary.style = "blackBorderUp"
  btnChangeSalary.callback = function() { ShowPlayerSalarySettings(); }

  this.updatePrimaryFunds();
}
