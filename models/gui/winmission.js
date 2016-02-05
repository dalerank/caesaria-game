function OnMissionContinue2Year()
{
  engine.log( "asdasasdasd" );
}

function OnMissionWin()
{
  var window = new Window( 0 );
  window.setGeometry( 0, 0, 540, 240 );
  window.setText( "##test_text##" );

  var btn = new PushButton( window );
  btn.setGeometry( 35, 140, window.width()-35, 160 );
  btn.setText( "##continue_2_years##" );
  btn.setFont( "FONT_2" );
  btn.setCallback( OnMissionContinue2Year );
  btn.checkCallback();

      //bgtype : "whiteBorderUp"

    //id : 2
    /*
    btnContinue5years#PushButton : {
      geometry : [ 35, 165, "pw-35", 185 ]
      bgtype : "whiteBorderUp"
      text : "##continue_5_years##"
      font : "FONT_2"
      id : 5
    }
    */
  engine.log( "bbbbbbbbbb" );
}

OnMissionWin();
