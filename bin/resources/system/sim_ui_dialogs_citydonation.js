function ShowPlayer2CityDonationWindow() {
  game.ui.dialogs.citydonation.show();
}

game.ui.dialogs.citydonation = {}

game.ui.dialogs.citydonation.show = function() {
  var ibox = g_ui.addSimpleWindow(0, 0, 510, 160);

  ibox.title = _u("send_money_to_city");
  ibox.lbBlack = ibox.addLabel(50, 50, ibox.w-100, 60);
  ibox.lbBlack.style = "blackFrame";
  ibox.lbBlack.font = "FONT_3";
  ibox.lbBlack.textAlign = { h:"center", v:"center" };
  ibox.wantSend = function(value) {
    ibox.denaries2send = math.clamp(value, 0, g_session.player.money() );
    ibox.lbDonation.text = _format( "{0} {1} from {2} dn", _ut("donation_is"), ibox.denaries2send, g_session.player.money() );
  }

  var btn0 = ibox.addButton(15, 10, 65, 20);
  btn0.font = "FONT_1";
  btn0.text = "0";
  btn0.tooltip = "##set_amount_to_donate##"
  btn0.style = "blackBorderUp"
  btn0.callback = function() { ibox.wantSend(0); }

  var btn500 = ibox.addButton( 95, 10, 65, 20)
  btn500.font = "FONT_1"
  btn500.text = "500"
  btn500.tooltip = "##set_amount_to_donate##"
  btn500.style = "blackBorderUp"
  btn500.callback = function() { ibox.wantSend(500); }

  var btn2000 = ibox.addButton(175, 10, 65, 20)
  btn2000.font = "FONT_1"
  btn2000.text = "2000"
  btn2000.tooltip = "##set_amount_to_donate##"
  btn2000.style = "blackBorderUp"
  btn2000.callback = function() { ibox.wantSend(2000); }

  var btnAll = ibox.addButton(255, 10, 65, 20);
  btnAll.font = "FONT_1"
  btnAll.text = "##send_all##"
  btnAll.tooltip = "##set_amount_to_donate##"
  btnAll.style = "blackBorderUp"
  btnAll.callback = function() { ibox.wantSend(g_session.player.money()); }

  lbDonation = ibox.addLabel(25, 32, 225, 24)
  lbDonation.text = "##donation_is##"
  lbDonation.textAlign = { h:"center", v:"center" }
  lbDonation.font : "FONT_2"
  ibox.lbDonation = lbDonation;

  var btnDecreaseDonation = ibox.addTexturedButton(255, 32, 24, 24)
  btnDecreaseDonation.states = { rc:"paneling", normal:601, hover:602, pressed:603, disabled:301 };
  btnDecreaseDonation.callback = function() {
    ibox.wantSend(ibox.denaries2send-10);
  }

  var btnIncreaseDonation = ibox.addTexturedButton(280, 32, 24, 24);
  btnIncreaseDonation.states = { rc:"paneling", normal:605, hover:606, pressed:607, disabled:305 };
  btnIncreaseDonation.callback = function() {
    ibox.wantSend(ibox.denaries2send+10);
  }

  var btnSend = ibox.addButton( 80, ibox.h-35, 160, 20)
  btnSend.font = "FONT_2"
  btnSend.text = "##give_money##"
  btnSend.tooltip = "##give_money_tip##"
  btnSend.style = "whiteBorderUp"
  btnSend.callback = function() {
    g_session.player.appendMoney(-ibox.denaries2send);
    g_session.city.createIssue("donation",ibox.denaries2send)

    //_updatePrimaryFunds();
    ibox.deleteLater();
  }

  var btnCancel = ibox.addButton(270, ibox.h-35, 160, 20);
  btnCancel.font = "FONT_2"
  btnCancel.text = "##cancel##"
  btnCancel.tooltip = "##donationwnd_exit_tip##"
  btnCancel.style = "whiteBorderUp"
  btnCancel.callback = function() { ibox.deleteLater(); }

  var player = g_session.player;
  if (player.money() == 0) {
    //for( auto& widget : lbBlack->children() )
    //  widget->hide();

    lbBlack.text = "##no_money_for_donation##";
  }

  btnSend.display = (player.money != 0);
}
