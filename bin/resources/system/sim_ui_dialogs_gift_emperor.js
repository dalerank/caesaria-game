game.ui.dialogs.gift2emperor = {}

game.ui.dialogs.gift2emperor.defaultGifts = [
    "##egift_chest_of_sapphire##",
    "##egift_golden_chariot##",
    "##egift_gree_manuscript##",
    "##egift_educated_slave##",
    "##egift_egyptian_glassware##",
    "##egift_gepards_and_giraffes##",
    "##egift_troupe_preforming_slaves##",
    "##egift_soldier_from_pergamaum##",
    "##egift_persian_carpets##"
  ]


game.ui.dialogs.gift2emperor.getCost = function(type, money)
{
  switch (type)
  {
  case g_config.gift.modest: return money / 8 + 20;
  case g_config.gift.generous: return money / 4 + 50;
  case g_config.gift.lavish: return money / 2 + 100;
  }

  return 100;
}

function OnShowEmperorGiftWindow() {
  game.ui.dialogs.gift2emperor.show()
}

game.ui.dialogs.gift2emperor.show = function()
{
  this.cost = 0

  var w = g_ui.addWindow(0, 0, 510, 260)
  w.title = "##dispatch_gift_title##"
  w.closeAfterKey( {escape:true, rmb:true} )
  w.moveToCenter()
  w.mayMove = false
  w.setModal()

  var lbxGifts = w.addListBox(16, 64, w.w-32, 126)
  lbxGifts.scrollbarVisible = false
  lbxGifts.textAlign = { v:"center", h:"center" }

  var gifts = this.defaultGifts.slice();
  var giftConfig = []
  for (var i=0; i<3; i++)
  {
    var rcost = this.getCost(i, maxMoney);
    var index = Math.random() * (gifts.length-1);

    var giftName = _t(gifts[index])
    if(giftName.length===0)
      continue;

    var index = lbxGifts.addLine(_format("{0} : {1}", giftName, rcost))
    lbxGifts.setItemOverrideColor( index, cost < g_session.player.money() ? g_config.colors.black : g_config.colors.grey )
    lbxGifts.setItemEnabled( index. cost < g_session.player.money() )
    giftConfig.push({name:giftName, cost:rcost})

    gifts.splice(index,1)
  }

  lbxGifts.onSelectedCallback = function(index) {
    this.cost = giftConfig[index].cost
  }

  var months2lastGift = g_session.emperor.lastGiftDate(g_session.city.name())
                                         .monthsTo(g_session.date)

  var lbLastGiftDate = w.addLabel(16, w.h-90, w.w-32, 20)
  lbLastGiftDate.textAlign = { v:"center", h:"center" }
  lbLastGiftDate.font = "FONT_2"
  lbLastGiftDate.text = months2lastGift > 100
                              ? _t( "##too_old_sent_gift##")
                              : _format( "{0}  {1}  {2}",
                                         _t("##time_since_last_gift##"),
                                         monthsLastGift,
                                         _t("##mo##") );

  var lbPlayerMoney = w.addLabel(16, w.h-70, w.w-32, 20)
  lbPlayerMoney.textAlign = { v:"center", h:"center" }
  lbPlayerMoney.text = _format( "{0} {1} Dn", _t("##you_have_money##"), g_session.player.money() )
  lbPlayerMoney.font = "FONT_2"

  var btnSend = w.addButton(34, w.h-40, 256, 20)
  btnSend.font = "FONT_2"
  btnSend.text = "##dispatch_gift##"
  btnSend.style = "whiteBorderUp"
  btnSend.callback = function() {
    if( this.cost > g_session.player.money() )
    {
      g_ui.addInformationDialog( "##nomoney_for_gift_title##", "##nomoney_for_gift_text##" );
      return;
    }

    g_session.player.removeMoney(this.cost)
    g_session.emperor.sengGift(g_session.city.name(), "gift", this.cost,g_session.date)
    w.deleteLater()
  }

  var btnCancel = w.addButton(340, w.h-40, 156, 20)
  btnCancel.font = "FONT_2"
  btnCancel.text = "##cancel##"
  btnCancel.style = "whiteBorderUp"
  btnCancel.callback = function() { w.deleteLater() }
}
