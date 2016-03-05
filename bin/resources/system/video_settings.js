var haveChanges = false;
var fullscreen = false;

function OnShowVideoSettings()
{
  engine.log( "JS:OnShowVideoSettings" );

  haveChanges = false;
  fullscreen = engine.getOption("fullscreen");

  var wnd = g_ui.addWindow(0,0,320,290);
  wnd.title = "##screen_settings##";
  wnd.closeAfterKey( {escape:true,rmb:true} );

  var btnSwitchMode = wnd.addButton( 25, 45, wnd.w-50, 20 );
  btnSwitchMode.text = fullscreen ? "##fullscreen_on##" : "##fullscreen_off##";
  btnSwitchMode.style = "smallGrayBorderLine";
  btnSwitchMode.callback = function() {
                                        fullscreen = !fullscreen;
                                        engine.setOption( "fullscreen", fullscreen );
                                        haveChanges = true;
                                        btnSwitchMode.text =  fullscreen ? "##fullscreen_on##" : "##fullscreen_off##";
                      };

  var lbxModes = wnd.addListbox( 25, 68, wnd.w-50, 160 );
  lbxModes.setTextAlignment( "center", "center" );
  lbxModes.background = true;
  lbxModes.onSelectedCallback = function(index) {
                        var mode = g_session.getVideoMode(index);
                        engine.log("w: " + mode.w + " h:" + mode.h );
                        g_session.resolution = mode;
                        haveChanges = true;
                    };

  for( var i=0; i < g_session.videoModesCount(); i++ )
  {
    var mode = g_session.getVideoMode(i);
    lbxModes.addLine( mode.w+"x"+mode.h );
  }

  var r = g_session.resolution;
  var index = lbxModes.findItem( r.w+"x"+r.h );
  lbxModes.selectedIndex = index;

  var btnOk = wnd.addButton( 25, 237, wnd.w-50, 20 );
  btnOk.style = "smallGrayBorderLine";
  btnOk.text = "##ok##";
  btnOk.callback = function() {
                if( haveChanges )
                  g_ui.addInformationDialog( "##pls_note##", "##need_restart_for_apply_changes##" );

                wnd.deleteLater();
            };
  btnOk.setFocus();

  wnd.moveToCenter();
  wnd.mayMove = false;
  wnd.setModal();
}
