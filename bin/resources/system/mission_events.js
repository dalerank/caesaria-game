function OnMissionWin(newTitle,winText,speech,mayContinue)
{ 
  var g_ui = new Ui();
  var g_session = new Session();
	
  engine.log( "JS:OnMissionWin" );

  var wnd = g_ui.addWindow(0,0,540,240);
  wnd.title = "##mission_win##";
  wnd.font = "FONT_5";

  var lbCaesarAssign = wnd.addLabel( 10, 40, wnd.width-20, 25 );
  lbCaesarAssign.text = "##caesar_assign_new_title##";
  lbCaesarAssign.font = "FONT_2";
  lbCaesarAssign.textAlign = { h:"center", v:"center" };
	
  var lbNewTitle = wnd.addLabel( 10, 70, wnd.width-20, 30 );
  lbNewTitle.text = newTitle;
  lbNewTitle.font = "FONT_5";
  lbNewTitle.textAlign = { h:"center", v:"center" };
	
  if( mayContinue )
  {
      var btn2years = wnd.addButton( 35, 140, wnd.width-70, 20 );
      btn2years.text = "##continue_2_years##";
      btn2years.font = "FONT_2";
      btn2years.style = "whiteBorderUp";
      btn2years.callback = function() {
                engine.log( "continue_2_years" );
                g_session.continuePlay(2);
                wnd.deleteLater();
            };

      var btn5years = wnd.addButton( 35, 165, wnd.width-70, 20 );
      btn5years.text = "##continue_5_years##";
      btn5years.font = "FONT_2";
      btn5years.style = "whiteBorderUp";
      btn5years.callback = function() {
                engine.log( "continue_5_years" );
                g_session.continuePlay(5);
                wnd.deleteLater();
            }
  }

  var btnAccept = wnd.addButton( 35, 115, wnd.width-70, 20 );
  btnAccept.text = "##accept_promotion##";
  btnAccept.font = "FONT_2";
  btnAccept.style = "whiteBorderUp";
  btnAccept.callback = function() {
            engine.log( "accept_promotion" );
            wnd.deleteLater();
            g_session.loadNextMission();
        };

  /*if (speech.length > 0)
    sound.play(speech);*/

  wnd.moveToCenter();
  wnd.setModal();
  wnd.mayMove = false;
	
  if (winText.length > 0)
    g_ui.addInformationDialog( "", winText );
}

function OnMissionStart()
{
  var g_ui = new Ui();
  var showAware = engine.getOption("showStartAware");
  if( showAware )
  {
    var dialog = g_ui.addInformationDialog( "##pls_note##", "##aware_black_objects##" );
    dialog.neverValue = true;
    dialog.onNeverCallback = function(value) {
				engine.setOption( "showStartAware", value );
			} 
  }
}

function OnRequestExitGame()
{
  var g_ui = new Ui();
  var dialog = g_ui.addConfirmationDialog( "", "##exit_without_saving_question##" );
  dialog.onYesCallback = function() {
                var g_session = new Session();
                g_session.quitGame();
            }
}

function OnShowEmpirePrices()
{
  var g_ui = new Ui();
  var g_session = new Session();
  engine.log( "JS:OnShowEmpirePrices" );

  var wnd = g_ui.addWindow(0,0,610,180);
  wnd.moveToCenter();
  wnd.title = "##rome_prices##";

  var lbBuyPrice = wnd.addLabel(10,84,140,104);
  lbBuyPrice.text = "##buy_price##";

  var lbSellPrice = wnd.addLabel(10, 108, 140, 128);
  lbSellPrice.text = "##sell_price##";

  var lbHint = wnd.addLabel(140, wnd.height-30, wnd.width-10, wnd.height-10 );
  lbHint.text = "##click_rmb_for_exit##";

  wnd.closeAfterKey({escape:true,rmb:true});

  var goods = g_session.tradableGoods();
  var pos = new Vector2D(130,50);
  var size = { w:24, h:24 };
  for (var i=0; i < goods.length; i++)
  {
    var goodInfo = g_session.getGoodInfo(goods[i]);
    engine.log( "pos.x="+pos.x+"    pos.y="+pos.y );
    var img = wnd.addImage(pos.x, pos.y, goodInfo.picture);
    img.tooltip = goodInfo.name;

    var relPos = pos.clone();
    relPos.add({x:0,y:34});
    
    var lb = wnd.addLabel(relPos.x, relPos.y, size.w, size.h);
    lb.text = goodInfo.importPrice;
    lb.font = "FONT_1";
    lb.textAlign = {h:"center", v:"center"};

    relPos = pos.clone();
    relPos.add({x:0,y:58});
    lb = wnd.addLabel(relPos.x, relPos.y, size.w, size.h);
    lb.text = goodInfo.exportPrice;
    lb.font = "FONT_1";
    lb.textAlign = {h:"center", v:"center"};
    lb.textColor = "darkOrange";

    pos.add({x:28,y:0});
  }

  wnd.mayMove = false;
  wnd.setModal();
}
