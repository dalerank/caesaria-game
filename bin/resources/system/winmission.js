s
var g_session = new _Session();

var OnMissionWin = function(newTitle,winText,speech,mayContinue)
{
  var wnd = g_ui.addWindow(30,30,540,240)
  wnd.geometry = { x:0, y:0, w:540, h:240 };
  wnd.title = "##mission_win##";
  wnd.font = "FONT_5";

  var lbNewTitle = wnd.addLabel( 10, 40, wnd.width-20, 20 );
  lbNewTitle.text = newTitle;
  lbNewTitle.font = "FONT_2";
  lbNewTitle.textAlign = { h:"center", v:"center" }

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
            }

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
            g_session.load( session.nextMission );
            wnd.deleteLater();
        }

   if (winText.length > 0)
       gui.addDialogInformation( winText );

   if (speech.length > 0)
       sound.play(speech);

   wnd.moveToCenter();
   wnd.setModal();
}
