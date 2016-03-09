function OnMissionLose()
{
   var window = g_ui.addWindow(0, 0, 400, 220);
	 window.title = "##mission_failed##";
   //window.titleFont = "FONT_6";

   var btnRestart = window.addButton(20, 120, 360, 24);
	 btnRestart.text = "##restart_mission##";
   btnRestart.tooltip = _t("##restart_mission_tip##");
	 btnRestart.callback = function() { g_session.setMode(g_config.level.res_restart) }
   
	 var btnMenu = window.addButton>(20, 150, 360, 24);
	 btnMenu.text = "##exit_to_main_menu##";
	 btnMenu.callback = function() { g_session.setMode(g_config.level.res_menu) }

   window.moveToCenter();
   window.setModal();
}

function OnMissionWin()
{
  var minfo = g_session.winConditions();
	var newTitle = minfo.title;
	var winText  = minfo.winText;
	var winSpeech = minfo.winSound;
	var mayContinue = minfo.winContinue;

  engine.log( "JS:OnMissionWin" );

  var wnd = g_ui.addWindow(0,0,540,240);
  wnd.title = "##mission_win##";
  wnd.font = "FONT_5";

  var lbCaesarAssign = wnd.addLabel(10, 40, wnd.w-20, 25);
  lbCaesarAssign.text = "##caesar_assign_new_title##";
  lbCaesarAssign.font = "FONT_2";
  lbCaesarAssign.textAlign = { h:"center", v:"center" };

  var lbNewTitle = wnd.addLabel( 10, 70, wnd.w-20, 30 );
  lbNewTitle.text = newTitle;
  lbNewTitle.font = "FONT_5";
  lbNewTitle.textAlign = { h:"center", v:"center" };

  if( mayContinue )
  {
      var btn2years = wnd.addButton( 35, 140, wnd.w-70, 20 );
      btn2years.text = "##continue_2_years##";
      btn2years.font = "FONT_2";
      btn2years.style = "whiteBorderUp";
      btn2years.callback = function() {
                engine.log( "continue_2_years" );
                g_session.continuePlay(2);
                wnd.deleteLater();
            };

      var btn5years = wnd.addButton( 35, 165, wnd.w-70, 20 );
      btn5years.text = "##continue_5_years##";
      btn5years.font = "FONT_2";
      btn5years.style = "whiteBorderUp";
      btn5years.callback = function() {
                engine.log( "continue_5_years" );
                g_session.continuePlay(5);
                wnd.deleteLater();
            }
  }

  var btnAccept = wnd.addButton( 35, 115, wnd.w-70, 20 );
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
  var showAware = engine.getOption("showStartAware");
  if( showAware )
  {
    var dialog = g_ui.addInformationDialog( "##pls_note##", "##aware_black_objects##" );
    dialog.neverValue = true;
    dialog.onNeverCallback = function(value) {
                engine.setOption( "showStartAware", value );
            }
  }

	var topmenu = sim.ui.topmenu;
  topmenu.init.file();
	topmenu.init.options();
  topmenu.init.help();
  topmenu.init.debug();
}

function OnShowSaveDialog()
{
  var savedir = g_session.getOptPath("savedir");
  var ext = engine.getOption("saveExt");

  if( !savedir.exist() )
  {
    g_ui.addInformationDialog( "##warning##", "##save_directory_not_exist##");
    return;
  }

  var dialog = g_ui.addSaveGameDialog(savedir, ext);
  dialog.callback = function(path) {
              g_session.save(path);
          }
}

function OnRequestExitGame()
{
  var dialog = g_ui.addConfirmationDialog( "", "##exit_without_saving_question##" );
  dialog.onYesCallback = function() { g_session.setMode(5); }
}

function OnShowEmpirePrices()
{
  engine.log( "JS:OnShowEmpirePrices" );

  var wnd = g_ui.addWindow(0,0,610,180);
  wnd.moveToCenter();
  wnd.title = "##rome_prices##";

  var lbBuyPrice = wnd.addLabel(10,84,140,104);
  lbBuyPrice.text = "##buy_price##";

  var lbSellPrice = wnd.addLabel(10, 108, 140, 128);
  lbSellPrice.text = "##sell_price##";

  var lbHint = wnd.addLabel(140, wnd.h-30, wnd.w-10, wnd.h-10 );
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
