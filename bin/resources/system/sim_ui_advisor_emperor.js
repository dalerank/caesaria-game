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
    widget.text = _format( "{0} {1} {2}", _ut("primary_funds"),
                                     g_session.player.money(),
                                     _ut("denarii_short"));
}

sim.ui.advisors.emperor.showRequestHistory = function() {
  var w = g_ui.addWindow(0, 0, 480, 640);
  w.title = "Request's history";
  w.addExitButton(w.w-37, 12);

  var abilities = g_city.relationAbilities;

  var listbox = w.addListBox(15, 45, w.w-15, w.h-15);
  listbox.itemHeight = 16;
  listbox.background = true;
  listbox.margin = {left:10, top:10};
  listbox.itemFont = "FONT_1";

  for (var i=0; i < abilities.length; i++) {
     var ability = abilities[i];
     if( ability.type == g_config.relationAbility.request ) {
       var text = _format( "{0} {1} {2}", DateTime.parse(ability.finished).romanStr,
                                          ability.successed ? "comply" : "failed",
                                          ability.message );
       var index = listbox.addLine( text );
       listbox.setItemTooltip(index, ability.message);
     }
   }

  w.moveToCenter();
  w.setModal();
  w.mayMove = false;
}

sim.ui.advisors.emperor.updateRequests = function(w) {
  var reqsRect = {x:32, y:91, w:570, h:220};

  for (var i=0; i < 10; i++) {
    var btn = w.find("#btnRequest"+i);
    if (btn != null) {
      btn.deleteLater();
      break;
    }
  }

  var requests = [];
  for (var i=0; i < 10; i++ ) {
    var t = g_session.getRequest(i);
    if (t == null)
      break;

    requests.push(t);
  }

  if (requests.length==0) {
    var label = w.addLabel(reqsRect.x, reqsRect.y, reqsRect.w, reqsRect.h);
    label.textAlign = {h:"center", v:"center"};
    label.text = _u("have_no_requests");
    label.multiline = true;
  } else {
    for (var i=0; i < requests.length; i++) {
      var r = requests[i];
      if (!r.isDeleted)
        w.addRequestButton(reqsRect.x+5,reqsRect.y+5, i, r);
    }
  }
}

sim.ui.advisors.emperor.showGiftsHistory = function() {
  var gifts = g_empire.emperor.getGifts(g_city.name);
  if (gifts.length==0) {
    g_ui.addInformationDialog( "Note", "You was not sent any gifts to emperor");
  } else {
    var w = g_ui.addWindow(0, 0, 480, 640);
    w.title = _u("history_gift");
    w.addExitButton(w.w-37, 12);

    var listbox = w.addListBox(15, 45, w.w-15, w.h-15);
    listbox.itemHeight = 16;
    listbox.background = true;
    listbox.margin = {left:10, top:10};
    listbox.itemFont = "FONT_1";

    for ( var i=0; i < gifts.length; i++) {
      var gift = gifts[i];
      var text = _format("{0} {1} {2}", DateTime.parse(gift.date).romanStr, gift.value, gift.name);
      listbox.fitText( text );
    }

    w.moveToCenter();
    w.setModal();
    w.mayMove = false;
  }
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

  w.btnHelp = w.addHelpButton(12, w.h - 36);
  w.btnHelp.uri = "emperor_advisor";

  var bgRequests = w.addLabel(32, 91, w.w-64, w.h-220);
  bgRequests.style = "blackFrame";

  var btnRqHistory = w.addButton(w.w-170, bgRequests.top()-28, 135, 24);
  btnRqHistory.text = "##rqs_history##";
  btnRqHistory.style = "blackBorderUp";

  var bgActions = w.addLabel(66, 315, 510, 90)
  bgActions.style = "blackFrame"

  var lbEmperorFavour = w.addLabel(58, 44, 550, 20);
  lbEmperorFavour.text = _format( "{0} {1}", _ut("advemp_emperor_favour"), g_city.favor )
  lbEmperorFavour.font = "FONT_2"

  var lbEmperorFavourDesc = w.addLabel(58, 64, 550, 20)
  lbEmperorFavourDesc.font = "FONT_2"
  lbEmperorFavourDesc.text = this.getEmperorFavourStr();

  var lbPost = w.addLabel(70, 325, 220, 20)
  lbPost.text = "Post"
  lbPost.font = "FONT_2"

  var btnGiftHistory = w.addButton(390, 325, 90, 20)
  btnGiftHistory.text = "##history_gift##"
  btnGiftHistory.style = "blackBorderUp"

  var btnSendGift = w.addButton(480, 325, 90, 20)
  btnSendGift.text = "##dispatch_gift##"
  btnSendGift.style = "blackBorderUp"
  btnSendGift.callback = function() { OnShowEmperorGiftWindow(); }

  var lbPrimaryFunds = w.addLabel(70, 355, 240, 20)
  lbPrimaryFunds.text = "##primary_funds##"
  lbPrimaryFunds.font = "FONT_2"
  lbPrimaryFunds.name = "#primaryFunds"

  var btnSend2City = w.addButton(322, 355, 250, 20)
  btnSend2City.text = "##send_to_city##"
  btnSend2City.style = "blackBorderUp"
  btnSend2City.callback = function() { ShowPlayer2CityDonationWindow(); }

  var btnChangeSalary = w.addButton(70, 380, 500, 20)
  btnChangeSalary.text = "##set_mayor_salary##"
  btnChangeSalary.style = "blackBorderUp"
  btnChangeSalary.callback = function() { ShowPlayerSalarySettings(); }

  w.addRequestButton = function(x, y, index, request) {
    var font = "FONT_2_WHITE";

    var ginfo = g_config.good.getInfo(request.gtype);
    engine.log(index)
    var month2comply = g_session.date.monthsTo(request.finishDate);
    var btn = w.addButton(x, y+45*index, 565, 40)
    btn.style = "blackBorderUp";
    btn.addLabel(8, 3, 25, 25, request.qty, {size:24, color:"brown"});
    btn.addLabel(60, 17, 60, 20, _u(ginfo.name), font);
    btn.addLabel(250, 2, 200, 20, _format( "{0} {1}",  month2comply, _ut("rqst_month_2_comply")), font );
    btn.addLabel(5, btn.w-20, 200, 20, request.description, font );
    btn.setEnabled(r.isReady);
    btn.name = "#btnRequest"+i;
    btn.tooltip = _u("request_btn_tooltip");

    var lbIcon = btn.addLabel(34, 5, 40, 40, "");
    lbIcon.icon = ginfo.picture.local;

    btn.callback = function() {
      var dialog = g_ui.addConfirmationDialog( "", _u("dispatch_emperor_request_question") );
      dialog.onYesCallback = function() { request.exec(); }
      sim.ui.advisors.emperor.updateRequests(w);
    }

    return btn;
  }

  this.updatePrimaryFunds();
  this.updateRequests(w);
}
