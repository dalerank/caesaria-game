function OnMissionWin()
{
  var wnd = gui.addWindow(30,30,540,240)
  wnd.geometry = { x:0, y:0, w:540, h:240 };
  wnd.title = "##mission_win##";
  wnd.font = "FONT_5";

  var lbNewTitle = wnd.addLabel( 10, 40, wnd.width-20, 20 );
  lbNewTitle.text = "##caesar_assign_new_title##";
  lbNewTitle.font = "FONT_2";
  lbNewTitle.textAlign = { h:"center", v:"center" }

  var btn2years = wnd.addButton( 35, 140, wnd.width-70, 20 );
  btn2years.text = "##continue_2_years##";
  btn2years.font = "FONT_2";
  btn2years.bgtype = "whiteBorderUp";
  btn2years.callback = function() {
            engine.log( "continue_2_years" );
            wnd.widget.deleteLater();
        }

  var btn5years = wnd.addButton( 35, 165, wnd.width-70, 20 );
  btn5years.text = "##continue_5_years##";
  btn5years.font = "FONT_2";
  btn5years.bgtype = "whiteBorderUp";
  btn5years.callback = function() {
            engine.log( "continue_5_years" );
            wnd.widget.deleteLater();
        }

  var btnAccept = wnd.addButton( 35, 115, wnd.width-70, 20 );
  btnAccept.text = "##accept_promotion##";
  btnAccept.font = "FONT_2";
  btnAccept.bgtype = "whiteBorderUp";
  btnAccept.callback = function() {
            engine.log( "accept_promotion" );
            wnd.widget.deleteLater();
        }
}

OnMissionWin();
